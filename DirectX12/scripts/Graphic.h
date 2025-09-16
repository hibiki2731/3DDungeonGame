#pragma once
#include "stdafx.h"
#include "Buffer.h"
#include "BIN_FILE12.h"

struct Vertex {
	XMFLOAT3 pos; //xyz座標
	XMFLOAT2 uv;  //uv座標
};

class Graphic
{
public:
	Graphic();

	void init();	
	HRESULT createBuf(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
	HRESULT updateBuf(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
	HRESULT mapBuf(void** mappedBuffer, ComPtr<ID3D12Resource>& buffer);
	void unmapBuf(ComPtr<ID3D12Resource>& buffer);
	UINT alignedSize(size_t size);
	HRESULT createShaderResource(const char* filename, ComPtr<ID3D12Resource>& shaderResource);
	HRESULT createDescriptorHeap(UINT numDescriptors);
	void createVertexBufferView(ComPtr<ID3D12Resource>& vertexBuf, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
	void createIndexBufferView(ComPtr<ID3D12Resource>& indexBuf, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
	UINT createConstantBufferView(ComPtr<ID3D12Resource>& constantBuf);
	UINT createShaderResourceView(ComPtr<ID3D12Resource>& shaderResource);
	
	void clearColor(float r, float g, float b);
	void beginRender();
	void drawMesh(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView, D3D12_INDEX_BUFFER_VIEW& indexBufferView, UINT cbvTbvIdx);
	void endRender();

	bool quit();
	int msg_wparam();
	void closeEventHandle();
	void waitGPU();

	//Getter
	float getAspect();

private:
	HRESULT createDevice();
	HRESULT createCommand();
	HRESULT createFence();
	HRESULT createWindow();
	HRESULT createSwapChain();
	HRESULT createBbv();
	HRESULT createDSbuf();
	HRESULT createDSbv();
	HRESULT createPipeline();


	

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
	MSG Msg;
	
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
	float ClearColor[4];

	//デプスステンシルバッファ
	ComPtr<ID3D12Resource> DepthStencilBuf;
	ComPtr<ID3D12DescriptorHeap> DsvHeap; //DepthStencilBufView
	//パイプライン
	ComPtr<ID3D12RootSignature> RootSignature;
	ComPtr<ID3D12PipelineState> PipelineState;
	D3D12_VIEWPORT Viewport;
	D3D12_RECT ScissorRect;

	//ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> CbvTbvHeap; //ConstBufferView, TextureBufferViewHeap
	UINT CurrentCbvTbvIndex;
	UINT CbvTbvIncSize;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);