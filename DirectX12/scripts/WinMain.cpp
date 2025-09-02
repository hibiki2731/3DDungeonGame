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
		//パイプライン
		{
			//ルートシグネチャ
			{
				D3D12_ROOT_SIGNATURE_DESC desc = {};
				desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

				//ルートシグネチャをシリアライズ blobを作る
				ComPtr<ID3DBlob> blob; 
				hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, blob.GetAddressOf(), nullptr);
				assert(SUCCEEDED(hr));

				//ルートシグネチャを作成
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
			UINT slot0 = 0;
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, slot0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
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
			blendDesc.RenderTarget[0].BlendEnable = FALSE;
			blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC1_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC1_ALPHA;
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
			static float radian = 0.0f;
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
			D3D12_VERTEX_BUFFER_VIEW vertexBufViews[] = { PositionBufView };
			CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			CommandList->IASetVertexBuffers(0, _countof(vertexBufViews), vertexBufViews);

			//描画
			CommandList->DrawInstanced(3, 1, 0, 0); //3頂点、1インスタンス
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

		}


	}

	//終了処理
	{
		WaitDrawDone();

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
