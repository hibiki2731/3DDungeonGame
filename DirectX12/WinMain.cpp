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
		HWND HWnd = CreateWindow(
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
		ShowWindow(HWnd, SW_SHOW);

		//メインループ
		while (true) {
			MSG msg = { 0 };
			if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}




		}

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



	return 0;	
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
