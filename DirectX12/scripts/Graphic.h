#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "Buffer.h"
#include "BIN_FILE12.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

struct Vertex {
	XMFLOAT3 pos; //xyz座標
	XMFLOAT2 uv;  //uv座標
};

class Graphic
{
public:
	enum STATE {
		RENDER3D,
		RENDER2D
	};

	Graphic();

	void init();	
	HRESULT createBuf(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
	HRESULT updateBuf(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
	HRESULT mapBuf(void** mappedBuffer, ComPtr<ID3D12Resource>& buffer);
	void unmapBuf(ComPtr<ID3D12Resource>& buffer);
	UINT alignedSize(UINT size);
	HRESULT createShaderResource(const std::string& filename, ComPtr<ID3D12Resource>& shaderResource);
	HRESULT createCbvTbvHeap(ComPtr<ID3D12DescriptorHeap>& cbvTbvHeap, UINT numDescriptors);
	HRESULT createSharedCbvTbvHeap(ComPtr<ID3D12DescriptorHeap>& cbvTbvHeap, UINT numDescriptors);
	void copySharedCbvTbvHeap(ComPtr<ID3D12DescriptorHeap> const& cbvTbvHeap, D3D12_CPU_DESCRIPTOR_HANDLE hDestHeap);
	void createVertexBufferView(ComPtr<ID3D12Resource> const& vertexBuf, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
	void createIndexBufferView(ComPtr<ID3D12Resource> const& indexBuf, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
	void createConstantBufferView(ComPtr<ID3D12Resource> const& constantBuf, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void createShaderResourceView(ComPtr<ID3D12Resource> const& shaderResource, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void createSharedConstBuf0();
	void updateViewProj(XMMATRIX& viewProj);
	void updateLightPos(XMFLOAT4& lightPos);
	
	void clearColor(float r, float g, float b);
	void beginRender();
	void endRender();

	bool quit();
	int msg_wparam();
	void closeEventHandle();
	void waitGPU();

	//Getter
	float getAspect();
	UINT getCbvTbvIncSize();
	ComPtr<ID3D12GraphicsCommandList>& getCommandList();
	ComPtr<ID3D12DescriptorHeap>& getCb0vHeap();

	//Setter
	void setRenderType(STATE state);

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
	ComPtr<ID3D12RootSignature> RootSignature2D;
	ComPtr<ID3D12PipelineState> PipelineState;
	ComPtr<ID3D12PipelineState> PipelineState2D;
	D3D12_VIEWPORT Viewport;
	D3D12_RECT ScissorRect;

	//コンスタントバッファ0
	CB0* CB0Data;
	ComPtr<ID3D12Resource> ConstBuf0;
	ComPtr<ID3D12DescriptorHeap> SharedCb0vHeap;

};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);