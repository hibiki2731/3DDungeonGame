#pragma once

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>
#include <cassert>
#include <wrl/client.h>
#include "stdafx.h"
using Microsoft::WRL::ComPtr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//ウィンドウ
LPCWSTR WindowTitle = L"DirectX12 Sample";
const int ClientWidth = 1280;
const int ClientHeight = 720;
const int ClientPosX = (GetSystemMetrics(SM_CXSCREEN) - ClientWidth) / 2;
const int ClientPosY = (GetSystemMetrics(SM_CYSCREEN) - ClientHeight) / 2;
#if 1
DWORD WindowStyle = WS_OVERLAPPEDWINDOW;
#else
DWORD WindowStyle = WS_POPUP;
#endif

HWND hWnd = nullptr;

//デバイス
ComPtr<ID3D12Device> Device;
//コマンド
ComPtr<ID3D12CommandAllocator> CommandAllocator;
ComPtr<ID3D12GraphicsCommandList> CommandList;
ComPtr<ID3D12CommandQueue> CommandQueue;
//フェンス
ComPtr<ID3D12Fence> Fence;
HANDLE FenceEvent;
UINT64 FenceValue;
//デバッグ
HRESULT hr;