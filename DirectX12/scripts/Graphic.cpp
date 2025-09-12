#include "Graphic.h"

void Graphic::init() {
	//ウィンドウの作成
	assert(SUCCEEDED(createWindow()));
	//デバイスの作成
	assert(SUCCEEDED(createDevice()));
	//コマンド作成
	assert(SUCCEEDED(createCommand()));
	//フェンス 処理完了のチェック
	assert(SUCCEEDED(createFence()));

	//スワップチェイン、バックバッファの作成
	assert(SUCCEEDED(createSwapChain()));
	//バックバッファビューの作成
	assert(SUCCEEDED(createBbv()));
	//デプスステンシルバッファの作成
	assert(SUCCEEDED(createDSbuf()));
	//デプスステンシルビューの作成
	assert(SUCCEEDED(createDSbv()));
}

HRESULT Graphic::createDevice() {
#ifdef _DEBUG
	//デバッグレイヤーをオンに
	ComPtr<ID3D12Debug> debug;
	HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	assert(SUCCEEDED(hr));
	debug->EnableDebugLayer();
#endif
	hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(Device.GetAddressOf())
	);
	return hr;
}

HRESULT Graphic::createCommand() {
	//コマンドアロケータ作成
	HRESULT hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CommandAllocator.GetAddressOf()));
	if(FAILED(hr))
	{
		return hr;
	}

	//コマンドリスト作成
	hr = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		CommandAllocator.Get(), nullptr, IID_PPV_ARGS(CommandList.GetAddressOf())
	);
	if (FAILED(hr)) {
		return hr;
	}

	//コマンドキュー作成
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	//GPUタイムアウトが有効
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; //直接コマンドキュー
	hr = Device->CreateCommandQueue(&desc, IID_PPV_ARGS(CommandQueue.GetAddressOf()));
	return hr;
}

HRESULT Graphic::createFence() {
	//GPUの処理完了をチェックするフェンスを作る
	HRESULT hr = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence.GetAddressOf()));
	if (FAILED(hr))
	{
		return hr;
	}
	FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
	FenceValue = 1;
	return hr;
}

HRESULT Graphic::createWindow() {
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

	return S_OK;
}

HRESULT Graphic::createSwapChain() {
	//DXGIファクトリーの生成、スワップチェイン作成(バックバッファもここで作成)
	ComPtr<IDXGIFactory4> dxgiFactory;
	HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
	if (FAILED(hr)) {
		return hr;
	}

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
	if (FAILED(hr)) {
		return hr;
	}

	//IDGISwapChain4のインターフェイスがサポートしているか確認、取得
	hr = swapChain1->QueryInterface(IID_PPV_ARGS(SwapChain.GetAddressOf()));
	return hr;
}

HRESULT Graphic::createBbv() {

	//デスクリプターヒープ（ビューを記憶する場所）の作成
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 2; //バックバッファの数
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //レンダーターゲットビュー
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //シェーダからアクセスしない
	HRESULT hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(BbvHeap.GetAddressOf()));
	assert(SUCCEEDED(hr));
	//バックバッファビューの作成
	
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
	return hr;
}

HRESULT Graphic::createDSbuf() {
	//デプスステンシルバッファを作る
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT; //DEFAULTだからあとはUNKNOWN
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = ClientWidth; //レンダーターゲットと同じ
	desc.Height = ClientHeight;
	desc.DepthOrArraySize = 1; //二次元のテクスチャデータとして　
	desc.Format = DXGI_FORMAT_D32_FLOAT; //深度書き込み用フォーマット
	desc.SampleDesc.Count = 1; //サンプルは1ピクセルあたり1つ
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //深度ステンシルとして使用
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.MipLevels = 1;
	//デプスステンシルバッファをクリアする値
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f; //深さ1(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; //32bit深度値としてクリア
	//デプスステンシルバッファを作る
	HRESULT hr = Device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //デプス書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(DepthStencilBuf.GetAddressOf())
	);
	return hr;
}

HRESULT Graphic::createDSbv() {
	//デプスステンシルバッファビューの入れ物であるディスクリプタヒープを作る
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 1; //深度ビュー1るのみ
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; //デプスステンシルビューのディスクリプタヒープ
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HRESULT hr = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(DsvHeap.GetAddressOf()));
		assert(SUCCEEDED(hr));
	}
	//デプスステンシルバッファビューをディスクリプタヒープに作る
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_D32_FLOAT; //デプス値に32bit
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; //2Dテクスチャ
		desc.Flags = D3D12_DSV_FLAG_NONE; //フラグなし
		D3D12_CPU_DESCRIPTOR_HANDLE hDsvHeap = DsvHeap->GetCPUDescriptorHandleForHeapStart();
		Device->CreateDepthStencilView(DepthStencilBuf.Get(), &desc, hDsvHeap);
	}
	return S_OK;
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

