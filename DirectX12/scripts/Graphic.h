#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cmath>
#include <cassert>
#include <fstream>
#include "BIN_FILE12.h"
#include <DirectXMath.h>/*
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"*/
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class Graphic
{
public:
	void init();
private:
	HRESULT createDevice();
	HRESULT createCommand();
	HRESULT createFence();
	HRESULT createWindow();
	HRESULT createSwapChain();
	HRESULT createBbv();
	HRESULT createDSbuf();
	HRESULT createDSbv();
	HRESULT createVertexBuf();


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
	HRESULT Hr;

	//リソース
	//バックバッファ
	ComPtr<IDXGISwapChain4> SwapChain;
	ComPtr<ID3D12Resource> BackBuffers[2];
	UINT BackBufIdx;
	ComPtr<ID3D12DescriptorHeap> BbvHeap; //BackBufferViewHeap
	UINT BbvHeapSize;
	//デプスステンシルバッファ
	ComPtr<ID3D12Resource> DepthStencilBuf;
	ComPtr<ID3D12DescriptorHeap> DsvHeap; //DepthStencilBufView
	//頂点位置バッファ
	ComPtr<ID3D12Resource> PositionBuf;
	D3D12_VERTEX_BUFFER_VIEW PositionBufView;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);