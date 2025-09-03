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
#include <DirectXMath.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using namespace DirectX;
using Microsoft::WRL::ComPtr;

void WaitDrawDone();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//ウィンドウ
LPCWSTR WindowTitle = L"DirectX12 Sample";
const int ClientWidth = 1280;
const int ClientHeight = 720;
const int ClientPosX = (GetSystemMetrics(SM_CXSCREEN) - ClientWidth) / 2;
const int ClientPosY = (GetSystemMetrics(SM_CYSCREEN) - ClientHeight) / 2;
const float Aspect = static_cast<float>(ClientWidth) / ClientHeight;
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
//頂点位置バッファ
ComPtr<ID3D12Resource> PositionBuf;
D3D12_VERTEX_BUFFER_VIEW PositionBufView;
//テクスチャ座標バッファ
ComPtr<ID3D12Resource> TexcoordBuf;
D3D12_VERTEX_BUFFER_VIEW TexcoordBufView;

//頂点インデックスバッファ
ComPtr<ID3D12Resource> IndexBuf;
D3D12_INDEX_BUFFER_VIEW IndexBufView;

//コンストバッファ0
struct CONST_BUF0 {
	XMMATRIX mat;
};
CONST_BUF0* MappedConstBuf0;
ComPtr<ID3D12Resource> ConstBuf0;
//コンスタバッファ1
struct CONST_BUF1 {
	XMFLOAT4 diffuse;
};
CONST_BUF1* MappedConstBuf1;
ComPtr<ID3D12Resource> ConstBuf1;
//テクスチャバッファ
ComPtr<ID3D12Resource> TextureBuf;
//ディスクリプタヒープ
ComPtr<ID3D12DescriptorHeap> CbvTbvHeap; //ConstBufferViewHeap

//パイプライン
ComPtr<ID3D12RootSignature> RootSignature;
ComPtr<ID3D12PipelineState> PipelineState;
D3D12_VIEWPORT Viewport;
D3D12_RECT ScissorRect;


