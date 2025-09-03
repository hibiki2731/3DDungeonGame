#include "WinMain.h"

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT) {
	//ウィンドウ
	{
		//ウィンドウクラスの登録
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WndProc;
		windowClass.hInstance = GetModuleHandle(nullptr);
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		windowClass.lpszClassName = L"DirectX12WindowClass";
		RegisterClassEx(&windowClass);
		//表示位置、ウィンドウの大きさ調節
		RECT windowRect = { 0, 0, ClientWidth, ClientHeight };
		AdjustWindowRect(&windowRect, WindowStyle, FALSE);
		int windowPosX = ClientPosX + windowRect.left;
		int windowPosY = ClientPosY + windowRect.top;
		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;
		//ウィンドウの生成
		hWnd = CreateWindow(
			windowClass.lpszClassName,
			WindowTitle,
			WindowStyle,
			windowPosX,
			windowPosY,
			windowWidth,
			windowHeight,
			nullptr,
			nullptr,
			windowClass.hInstance,
			nullptr
		);
		ShowWindow(hWnd, SW_SHOW);
	}

	//デバイス
	{
#ifdef _DEBUG
		//デバッグレイヤーをオンに
		ComPtr<ID3D12Debug> debug;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
		assert(SUCCEEDED(hr));
		debug->EnableDebugLayer();
#endif
		//デバイスの作成
		{
			hr = D3D12CreateDevice(
				nullptr,
				D3D_FEATURE_LEVEL_12_0,
				IID_PPV_ARGS(Device.GetAddressOf())
			);
			assert(SUCCEEDED(hr));
		}
		//コマンド
		{
			//コマンドアロケータ作成
			hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(CommandAllocator.GetAddressOf()));
			assert(SUCCEEDED(hr));

			//コマンドリスト作成
			hr = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
				CommandAllocator.Get(), nullptr, IID_PPV_ARGS(CommandList.GetAddressOf())
			);
			assert(SUCCEEDED(hr));

			//コマンドキュー作成
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	//GPUタイムアウトが有効
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //直接コマンドキュー
			hr = Device->CreateCommandQueue(&desc, IID_PPV_ARGS(CommandQueue.GetAddressOf()));
			assert(SUCCEEDED(hr));
		}

		//フェンス 処理完了のチェック
		{
			//GPUの処理完了をチェックするフェンスを作る
			hr = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence.GetAddressOf()));
			assert(SUCCEEDED(hr));
			FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
			assert(FenceEvent != nullptr);
			FenceValue = 1;
		}
	}

	//リソース
	{
		//DXGIファクトリーの生成
		ComPtr<IDXGIFactory4> dxgiFactory;
		hr = CreateDXGIFactory2(0, IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
		assert(SUCCEEDED(hr));

		//スワップチェインの作成
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.BufferCount = 2; //ダブルバッファ
		desc.Width = ClientWidth;
		desc.Height = ClientHeight;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //色の書式
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //フリップ後は保証しない（その代わり早い）
		desc.SampleDesc.Count = 1;
		ComPtr<IDXGISwapChain1> swapChain1;
		hr = dxgiFactory->CreateSwapChainForHwnd(
			CommandQueue.Get(), hWnd, &desc, nullptr, nullptr, swapChain1.GetAddressOf()
		);
		assert(SUCCEEDED(hr));

		//IDGISwapChain4のインターフェイスがサポートしているか確認、取得
		hr = swapChain1->QueryInterface(IID_PPV_ARGS(SwapChain.GetAddressOf()));
		assert(SUCCEEDED(hr));

		//デスクリプターヒープ（ビューを記憶する場所）の作成
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.NumDescriptors = 2; //バックバッファの数
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //レンダーターゲットビュー
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //シェーダからアクセスしない
			hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(BbvHeap.GetAddressOf()));
			assert(SUCCEEDED(hr));
		}
		//バックバッファビューの作成
		{
			//ポインタにヒープのアドレスを入れる
			D3D12_CPU_DESCRIPTOR_HANDLE hBbvHeap = BbvHeap->GetCPUDescriptorHandleForHeapStart();
			//ビューのサイズ
			BbvHeapSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			for (UINT idx = 0; idx < 2; idx++) {
				//スワップチェイン内のバックバッファを取り出す
				hr = SwapChain->GetBuffer(idx, IID_PPV_ARGS(BackBuffers[idx].GetAddressOf()));
				assert(SUCCEEDED(hr));
				//バックバッファのビューを作成
				hBbvHeap.ptr += idx * BbvHeapSize;
				Device->CreateRenderTargetView(BackBuffers[idx].Get(), nullptr, hBbvHeap);
			}
		}
		//頂点バッファ　位置
		{
			//位置の生データ
			float position[] = {
				-0.5f, -0.5f, 0.0f,
				-0.5f, 0.5f, 0.0f,
				0.5f, -0.5f, 0.0f,
				0.5f, 0.5f, 0.0f,
			};

			//位置のバッファを作成
			//メモリの属性
			D3D12_HEAP_PROPERTIES prop = {};
			prop.Type = D3D12_HEAP_TYPE_UPLOAD; //CPUからGPUへ書き込み
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			prop.CreationNodeMask = 1;
			prop.VisibleNodeMask = 1;
			D3D12_RESOURCE_DESC desc = {};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //一次元配列
			desc.Alignment = 0;
			desc.Width = sizeof(position); //バッファのサイズ
			desc.Height = 1;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			hr = Device->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(PositionBuf.GetAddressOf())
			);
			assert(SUCCEEDED(hr));

			//位置データをバッファにコピー
			UINT8* mappedPositionBuf; //コピー先のアドレス
			hr = PositionBuf->Map(0, nullptr, reinterpret_cast<void**>(&mappedPositionBuf));
			assert(SUCCEEDED(hr));
			memcpy(mappedPositionBuf, position, sizeof(position));
			PositionBuf->Unmap(0, nullptr);

			//位置バッファのビューを初期化（ディスクリプタヒープに作らなくて良い）
			PositionBufView.BufferLocation = PositionBuf->GetGPUVirtualAddress();
			PositionBufView.SizeInBytes = sizeof(position); //全バイト数
			PositionBufView.StrideInBytes = sizeof(float) * 3; //1頂点のバイト数

		}
		//頂点バッファ、テクスチャ座標
		{
			//テクスチャ座標の生データ
			float texcoord[] = {
				0.0f, 1.0f,
				0.0f, 0.0f,
				1.0f, 1.0f,
				1.0f, 0.0f,
			};

			//テクスチャ座標のバッファを作成
			D3D12_HEAP_PROPERTIES prop = {};
			prop.Type = D3D12_HEAP_TYPE_UPLOAD; //CPUからGPUへ書き込み
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			prop.CreationNodeMask = 1;
			prop.VisibleNodeMask = 1;
			D3D12_RESOURCE_DESC desc = {};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //一次元配列
			desc.Alignment = 0;
			desc.Width = sizeof(texcoord); //バッファのサイズ
			desc.Height = 1;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			hr = Device->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(TexcoordBuf.GetAddressOf())
			);
			assert(SUCCEEDED(hr));

			//テクスチャ座標データをバッファにコピー
			UINT8* mappedTexcoordBuf; //コピー先のアドレス
			hr = TexcoordBuf->Map(0, nullptr, reinterpret_cast<void**>(&mappedTexcoordBuf));
			assert(SUCCEEDED(hr));
			memcpy(mappedTexcoordBuf, texcoord, sizeof(texcoord));
			TexcoordBuf->Unmap(0, nullptr);

			//テクスチャ座標バッファのビューを初期化（ディスクリプタヒープに作らなくて良い）
			TexcoordBufView.BufferLocation = TexcoordBuf->GetGPUVirtualAddress();
			TexcoordBufView.SizeInBytes = sizeof(texcoord); //全バイト数
			TexcoordBufView.StrideInBytes = sizeof(float) * 2; //1頂点のバイト数
		}
		//インデックスバッファ
		{
			unsigned short indices[] = {
				0,1,2,
				2,1,3,
			};

			//インデックスバッファの作成（バーテックスバッファと同じ）
			D3D12_HEAP_PROPERTIES prop = {};
			prop.Type = D3D12_HEAP_TYPE_UPLOAD;
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			prop.CreationNodeMask = 1;
			prop.VisibleNodeMask = 1;
			D3D12_RESOURCE_DESC desc = {};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //バッファーを指令（一次元配列）
			desc.Alignment = 0;
			desc.Width = sizeof(indices);
			desc.Height = 1;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			hr = Device->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(IndexBuf.GetAddressOf())
			);
			assert(SUCCEEDED(hr));

			//作ったバッファにデータをコピー
			UINT8* mappedIndexBuf = nullptr;
			hr = IndexBuf->Map(0, nullptr, (void**)&mappedIndexBuf);
			assert(SUCCEEDED(hr));
			memcpy(mappedIndexBuf, indices, sizeof(indices));
			IndexBuf->Unmap(0, nullptr);

			//インデックスバッファビューの作成
			IndexBufView.BufferLocation = IndexBuf->GetGPUVirtualAddress();
			IndexBufView.SizeInBytes = sizeof(indices);
			IndexBufView.Format = DXGI_FORMAT_R16_UINT; //unsigned short
		} 
		//コンスタントバッファとディスクリプタヒープ
		{
			//コンスタントバッファ0を作成
			{
				D3D12_HEAP_PROPERTIES prop = {};
				prop.Type = D3D12_HEAP_TYPE_UPLOAD; //CPUからGPUへ書き込み
				prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				prop.CreationNodeMask = 1;
				prop.VisibleNodeMask = 1;
				D3D12_RESOURCE_DESC desc = {};
				desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //一次元配列
				desc.Alignment = 0;
				desc.Width = 256; //コンスタントバッファは256の倍数バイトにする（必須）
				desc.Height = 1;
				desc.DepthOrArraySize = 1;
				desc.MipLevels = 1;
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.SampleDesc = { 1,0 };
				desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				desc.Flags = D3D12_RESOURCE_FLAG_NONE;
				hr = Device->CreateCommittedResource(
					&prop,
					D3D12_HEAP_FLAG_NONE,
					&desc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(ConstBuf0.GetAddressOf())
				);
				assert(SUCCEEDED(hr));

				hr = ConstBuf0->Map(0, nullptr, (void**)&MappedConstBuf0); //マップしっぱなしにする
				assert(SUCCEEDED(hr));
			}
			//コンスタントバッファ1を作成
			{
				D3D12_HEAP_PROPERTIES prop = {};
				prop.Type = D3D12_HEAP_TYPE_UPLOAD; //CPUからGPUへ書き込み
				prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				prop.CreationNodeMask = 1;
				prop.VisibleNodeMask = 1;
				D3D12_RESOURCE_DESC desc = {};
				desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //一次元配列
				desc.Alignment = 0;
				desc.Width = 256; //コンスタントバッファは256の倍数バイトにする（必須）
				desc.Height = 1;
				desc.DepthOrArraySize = 1;
				desc.MipLevels = 1;
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.SampleDesc = { 1,0 };
				desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				desc.Flags = D3D12_RESOURCE_FLAG_NONE;
				hr = Device->CreateCommittedResource(
					&prop,
					D3D12_HEAP_FLAG_NONE,
					&desc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(ConstBuf1.GetAddressOf())
				);
				assert(SUCCEEDED(hr));

				//マップしておいて、メインループ中に更新
				hr = ConstBuf1->Map(0, nullptr, (void**)&MappedConstBuf1);
				assert(SUCCEEDED(hr));
			}
			//テクスチャバッファを作る(Unified Memory Accessバージョン)
			{
				//ファイル読み込み、画像データを取り出す
				unsigned char* pixels = nullptr;
				int width = 0, height = 0, bytePerPixel = 4;
				pixels = stbi_load("assets\\penguin1.png", &width, &height, nullptr, bytePerPixel);
				assert(pixels != nullptr);

				//テクスチャバッファの作成
				D3D12_HEAP_PROPERTIES prop = {};
				prop.Type = D3D12_HEAP_TYPE_CUSTOM;
				prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE; //ライトバックで
				prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //転送がL0から
				prop.CreationNodeMask = 0; //単一アダプタのため0
				prop.VisibleNodeMask = 0; //単一アダプタのため0
				D3D12_RESOURCE_DESC desc = {};
				desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //2Dテクスチャ
				desc.Alignment = 0;
				desc.Width = width;
				desc.Height = height;
				desc.DepthOrArraySize = 1;
				desc.MipLevels = 1;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //RGBA8
				desc.SampleDesc.Count = 1;
				desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				desc.Flags = D3D12_RESOURCE_FLAG_NONE;
				hr = Device->CreateCommittedResource(
					&prop,
					D3D12_HEAP_FLAG_NONE,
					&desc,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, //テクスチャ用
					nullptr,
					IID_PPV_ARGS(TextureBuf.GetAddressOf())
				);
				assert(SUCCEEDED(hr));

				//テクスチャバッファに画像データをコピー
				hr = TextureBuf->WriteToSubresource(
					0,
					nullptr, //全領域にコピー
					pixels,
					width * bytePerPixel, //1ラインのバイト数
					width * height * bytePerPixel //全データのバイト数
				);

				//生データを解法
				stbi_image_free(pixels);
			}
			//ディスクリプタヒープ
			{
				//ディスクリプタヒープを作成
				{
					D3D12_DESCRIPTOR_HEAP_DESC desc = {};
					desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; //定数バッファビュー、シェーダリソースビュー、アンオーダードアクセスビュー
					desc.NumDescriptors = 3; //コンスタントバッファ2つとテクスチャ1つ
					desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダから見えるように
					hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(CbvTbvHeap.GetAddressOf()));
					assert(SUCCEEDED(hr));
				}

				auto hCbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart(); //ヒープの先頭アドレス

				//コンスタントバッファ0のビューをディスクリプタヒープに作成
				{
					D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
					desc.BufferLocation = ConstBuf0->GetGPUVirtualAddress();
					desc.SizeInBytes = static_cast<UINT>(ConstBuf0->GetDesc().Width); //256バイトアラインメント
					Device->CreateConstantBufferView(&desc, hCbvHeap);
				}

				auto heapSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				hCbvHeap.ptr += heapSize; //次のディスクリプタのアドレス

				//コンスタントバッファ1のビューをディスクリプタヒープに作成
				{
					D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
					desc.BufferLocation = ConstBuf1->GetGPUVirtualAddress();
					desc.SizeInBytes = static_cast<UINT>(ConstBuf1->GetDesc().Width); //256バイトアラインメント
					Device->CreateConstantBufferView(&desc, hCbvHeap);
				}

				hCbvHeap.ptr += heapSize; //次のディスクリプタのアドレス
				//テクスチャバッファビューをディスクリプタヒープに作成
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
					desc.Format = TextureBuf->GetDesc().Format;
					desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					desc.Texture2D.MipLevels = 1;
					Device->CreateShaderResourceView(TextureBuf.Get(), &desc, hCbvHeap);
				}

			}
		}
		//パイプライン
		{
			//ルートシグネチャ
			{
				//ディスクリプタレンジ、ディスクリプタヒープとシェーダを紐づける役割を持つ
				D3D12_DESCRIPTOR_RANGE range[3] = {};
				UINT b0 = 0;
				range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //定数バッファビュー
				range[0].BaseShaderRegister = b0;
				range[0].NumDescriptors = 1; //b0だけ
				range[0].RegisterSpace = 0;
				range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //自動計算

				UINT b1 = 1;
				range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //定数バッファビュー
				range[1].BaseShaderRegister = b1;
				range[1].NumDescriptors = 1; //b1だけ
				range[1].RegisterSpace = 0;
				range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //自動計算

				UINT t0 = 0;
				range[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //シェーダリソースビュー
				range[2].BaseShaderRegister = t0;
				range[2].NumDescriptors = 1; //t0だけ
				range[2].RegisterSpace = 0;
				range[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //自動計算

				//ルートパラメタをディスクリプタテーブルとして使用
				//rangeの入れ物
				D3D12_ROOT_PARAMETER rootParam[1] = {};
				rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				rootParam[0].DescriptorTable.pDescriptorRanges = range;
				rootParam[0].DescriptorTable.NumDescriptorRanges = _countof(range);
				rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全てのシェーダから見える

				//サンプラの記述
				D3D12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
				samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT; //補完しない
				samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //U方向は繰り返し
				samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //V方向は繰り返し
				samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //W方向は繰り返し
				samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
				samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX; //ミップマップ最大
				samplerDesc[0].MinLOD = 0.0f; //ミップマップ最小
				samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //オーバーサンプリングの際リサンプリングしない
				samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //ピクセルシェーダからのみ見える

				//ルートシグネチャの設定
				D3D12_ROOT_SIGNATURE_DESC desc = {};
				desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; //入力アセンブラの入力レイアウトを許可
				desc.pParameters = rootParam;
				desc.NumParameters = _countof(rootParam);
				desc.pStaticSamplers = samplerDesc;  //サンプラーの先頭アドレス
				desc.NumStaticSamplers = _countof(samplerDesc); //サンプラーの数

				//ルートシグネチャをシリアライズ(コンパイルするようなもの)
				ComPtr<ID3DBlob> blob;
				hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, blob.GetAddressOf(), nullptr);
				assert(SUCCEEDED(hr));

				//ルートシグネチャの作成
				hr = Device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
					IID_PPV_ARGS(RootSignature.GetAddressOf()));
				assert(SUCCEEDED(hr));
			}

			//シェーダの読み込み
			BIN_FILE12 vs("assets\\VertexShader.cso");
			assert(vs.succeeded());
			BIN_FILE12 ps("assets\\PixelShader.cso");
			assert(ps.succeeded());

			//各種記述
			UINT slot0 = 0, slot1 = 1;
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, slot0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, slot1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			};

			D3D12_RASTERIZER_DESC rasterDesc = {};
			rasterDesc.FrontCounterClockwise = FALSE; //時計回り
			rasterDesc.CullMode = D3D12_CULL_MODE_NONE; //裏面描画するか？
			rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
			rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			rasterDesc.DepthClipEnable = TRUE;
			rasterDesc.MultisampleEnable = FALSE;
			rasterDesc.AntialiasedLineEnable = FALSE;
			rasterDesc.ForcedSampleCount = 0;
			rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

			D3D12_BLEND_DESC blendDesc = {};
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; //書き込み許可
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; //小さいほうが手前
			depthStencilDesc.StencilEnable = FALSE; //ステンシルしない

			//ここまでの記述をまとめてパイプラインステートオブジェクトを作成
			D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
			pipelineDesc.pRootSignature = RootSignature.Get();
			pipelineDesc.VS = { vs.code(), vs.size() };
			pipelineDesc.PS = { ps.code(), ps.size() };
			pipelineDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			pipelineDesc.RasterizerState = rasterDesc;
			pipelineDesc.BlendState = blendDesc;
			pipelineDesc.DepthStencilState = depthStencilDesc;
			pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			pipelineDesc.SampleMask = UINT_MAX;
			pipelineDesc.SampleDesc.Count = 1;
			pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			pipelineDesc.NumRenderTargets = 1;
			pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			hr = Device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(PipelineState.GetAddressOf()));
			assert(SUCCEEDED(hr));

			//出力領域を設定
			Viewport.TopLeftX = 0.0f;
			Viewport.TopLeftY = 0.0f;
			Viewport.Width = ClientWidth;
			Viewport.Height = ClientHeight;
			Viewport.MinDepth = 0.0f;
			Viewport.MaxDepth = 1.0f;

			//切り取り矩形を設定
			ScissorRect.left = 0;
			ScissorRect.top = 0;
			ScissorRect.right = ClientWidth;
			ScissorRect.bottom = ClientHeight;


		}
	}
	

	//メインループ
	while (true) {
		//ウィンドウメッセージの取得、送出
		{
			MSG msg = { 0 };
			if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
		}

		//回転用ラジアン
		static float radian = 0;
		radian += 0.01f;
		//ワールドマトリックス
		XMMATRIX world = XMMatrixRotationY(radian);
		//ビューマトリックス
		XMVECTOR eye = { 0.0f, 0.0f, -2.0f }, focus = { 0.0f, 0.0f, 0.0f }, up = { 0.0f , 1.0f, 0.0f };
		XMMATRIX view = XMMatrixLookAtLH(eye, focus, up);
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, Aspect, 1.0f, 10.0f);
		//コンスタントバッファの更新
		MappedConstBuf0->mat = world * view * proj;

		//コンストバッファ1の更新
		float col = cos(radian) * 0.5f + 0.5f;
		MappedConstBuf1->diffuse = { col, 1.0f, 0.0f, 1.0f };

		//バッファのクリア
		{
			//現在のバックバッファのインデックスを取得
			BackBufIdx = SwapChain->GetCurrentBackBufferIndex();

			//バリアでバックバッファを描画ターゲットに切り替える
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; //状態遷移タイプ
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = BackBuffers[BackBufIdx].Get(); //リソースはバックバッファ
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; //切換前は表示状態
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	 //切換後は描画ターゲット
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			CommandList->ResourceBarrier(1, &barrier);

			//バックバッファの場所を示すディスクリプタヒープハンドルを用意
			auto hBbvHeap = BbvHeap->GetCPUDescriptorHandleForHeapStart();
			hBbvHeap.ptr += BackBufIdx * BbvHeapSize;
			//バックバッファを描画ターゲットとして設定
			CommandList->OMSetRenderTargets(1, &hBbvHeap, FALSE, nullptr);
			//描画ターゲットをクリア
			static float radian = 0;
			float r = cos(radian) * 0.5f + 0.5f;
			float g = 0.25f;
			float b = 0.5f;
			const float clearColor[] = { r, g, b, 1.0f }; //青っぽい色
			radian += 0.01f;
			CommandList->ClearRenderTargetView(hBbvHeap, clearColor, 0, nullptr);

		}

		//バックバッファに描画
		{
			//パイプラインステートをセット
			CommandList->SetPipelineState(PipelineState.Get());
			//ビューポート、シザー矩形をセット
			CommandList->RSSetViewports(1, &Viewport);
			CommandList->RSSetScissorRects(1, &ScissorRect);
			//ルートシグネチャをセット
			CommandList->SetGraphicsRootSignature(RootSignature.Get());

			//頂点をセット
			D3D12_VERTEX_BUFFER_VIEW vertexBufViews[] = { PositionBufView, TexcoordBufView };
			CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			CommandList->IASetVertexBuffers(0, _countof(vertexBufViews), vertexBufViews);
			CommandList->IASetIndexBuffer(&IndexBufView);

			//ディスクリプタヒープをGPUにセット。
			CommandList->SetDescriptorHeaps(1, CbvTbvHeap.GetAddressOf());
			//ディスクリプタヒープをディスクリプタテーブルにセット、シェーダと紐づけ
			auto hCbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
			CommandList->SetGraphicsRootDescriptorTable(0, hCbvHeap);

			//描画
			CommandList->DrawIndexedInstanced(6, 1, 0, 0, 0); //(インデックスの数、オブジェクトの数)
		}

		//バックバッファを表示
		{
			//バリアでバックバッファを表示用に切り替える
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; //状態遷移タイプ
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = BackBuffers[BackBufIdx].Get(); //リソースはバックバッファ
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; //切換前は描画ターゲット
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;	 //切換後は表示用
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			CommandList->ResourceBarrier(1, &barrier);

			//コマンドリのクローズ
			CommandList->Close();	//コマンドをため込むのを終了
			//コマンドリストの実行
			ComPtr<ID3D12CommandList> commandLists[] = { CommandList.Get() };
			CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists->GetAddressOf());  //GPU上で行われる
			//描画完了を待つ
			WaitDrawDone();

			//バックバッファを表示
			SwapChain->Present(1, 0); //1は垂直同期あり、0はなし

			//コマンドアロケータをリセット
			hr = CommandAllocator->Reset();
			assert(SUCCEEDED(hr));
			//コマンドリストをリセット
			hr = CommandList->Reset(CommandAllocator.Get(), nullptr); //PSOをセットするならここで指定
			assert(SUCCEEDED(hr));

		}


	}

	//終了処理
	{
		WaitDrawDone();
		ConstBuf0->Unmap(0, nullptr);
		ConstBuf1->Unmap(0, nullptr);
		CloseHandle(FenceEvent);
	}
	return 0;	
}

void WaitDrawDone() {
	//fvalueがコマンド終了後にFenceに書き込まれるようにする
	UINT64 fvalue = FenceValue;
	CommandQueue->Signal(Fence.Get(), fvalue);
	FenceValue++;

	//まだコマンドキューが終了していないことを確認する
	if (Fence->GetCompletedValue() < fvalue) {
		//Fenceがfvalueの値になったら終了を通知するイベントを発生させる
		Fence->SetEventOnCompletion(fvalue, FenceEvent);
		//終了通知イベントが発生するまで待つ
		WaitForSingleObject(FenceEvent, INFINITE);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
