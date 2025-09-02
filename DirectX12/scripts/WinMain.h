#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <dxgi1_6.h>
#include <cmath>
#include <d3d12.h>
#include <cassert>
#include <wrl/client.h>
#include <fstream>
#include "BIN_FILE12.h"
#include "stdafx.h"
using Microsoft::WRL::ComPtr;

void WaitDrawDone();

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

//リソース
//バックバッファ
ComPtr<IDXGISwapChain4> SwapChain;
ComPtr<ID3D12Resource> BackBuffers[2];
UINT BackBufIdx;
ComPtr<ID3D12DescriptorHeap> BbvHeap; //BackBufferViewHeap
UINT BbvHeapSize;
//頂点バッファ
ComPtr<ID3D12Resource> PositionBuf;
D3D12_VERTEX_BUFFER_VIEW PositionBufView;
//パイプライン
ComPtr<ID3D12RootSignature> RootSignature;
ComPtr<ID3D12PipelineState> PipelineState;
D3D12_VIEWPORT Viewport;
D3D12_RECT ScissorRect;

