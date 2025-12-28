#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite.lib")
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include <dxgi1_6.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <vector>
#include "Definition.h"
#include "BIN_FILE12.h"
#include <winuser.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class PointLightComponent;
class SpotLightComponent;

struct Vertex {
	XMFLOAT3 pos; //xyz座標
	XMFLOAT2 uv;  //uv座標
};

class Graphic
{
public:
	enum STATE {
		RENDER_3D,
		RENDER_2D,
	};

	Graphic();

	void init();	
	HRESULT createBuf(UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
	HRESULT updateBuf(void* data, UINT sizeInBytes, ComPtr<ID3D12Resource>& buffer);
	HRESULT mapBuf(void** mappedBuffer, ComPtr<ID3D12Resource>& buffer);
	void unmapBuf(ComPtr<ID3D12Resource>& buffer);
	UINT alignedSize(UINT size);
	HRESULT createShaderResource(const std::string& filename, ComPtr<ID3D12Resource>& shaderResource);
	XMFLOAT2 createShaderResourceGetSize(const std::string& filename, ComPtr<ID3D12Resource>& shaderResource);
	HRESULT createCbvTbvHeap(ComPtr<ID3D12DescriptorHeap>& cbvTbvHeap, UINT numDescriptors);
	HRESULT createSharedCbvTbvHeap(ComPtr<ID3D12DescriptorHeap>& cbvTbvHeap, UINT numDescriptors);
	void copySharedCbvTbvHeap(ComPtr<ID3D12DescriptorHeap> const& cbvTbvHeap, D3D12_CPU_DESCRIPTOR_HANDLE hDestHeap);
	void createVertexBufferView(ComPtr<ID3D12Resource> const& vertexBuf, UINT sizeInBytes, UINT strideInBytes, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
	void createIndexBufferView(ComPtr<ID3D12Resource> const& indexBuf, UINT sizeInBytes, D3D12_INDEX_BUFFER_VIEW& indexBufferView);
	void createConstantBufferView(ComPtr<ID3D12Resource> const& constantBuf, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void createShaderResourceView(ComPtr<ID3D12Resource> const& shaderResource, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void createSharedConstBuf0();
	void updateViewProj(XMMATRIX& viewProj);
	void updatePointLight(std::vector<std::shared_ptr<PointLightComponent>>& lights);
	void updateSpotLight(std::vector<std::shared_ptr<SpotLightComponent>>& lights);
	void updateCameraPos(XMFLOAT4& cameraPos);
	
	void clearColor(float r, float g, float b);
	void begin3DRender();
	void end3DRender();
	void begin2DRender();
	void end2DRender();
	void moveToNextFrame();

	bool quit();
	int msg_wparam();
	void closeEventHandle();
	void waitGPU();

	//Getter
	float getAspect();
	UINT getCbvTbvIncSize();
	ComPtr<ID3D12GraphicsCommandList>& getCommandList();
	ComPtr<ID3D12CommandQueue>& getCommandQueue();
	ComPtr<ID3D12CommandAllocator>& getCommandAllocator();
	ComPtr<ID3D12DescriptorHeap>& getCb0vHeap();
	ComPtr<ID3D12Device>& getDevice();
	float getClientWidth();
	float getClientHeight();
	ComPtr<ID2D1DeviceContext>& getD2DDeviceContext();
	ComPtr<IDWriteFactory>& getDWriteFactory();
	ComPtr<ID2D1Bitmap1>& getD2DRenderTarget();

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
	HRESULT createD2D();


	

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
	ComPtr<ID2D1Device> mD2DDevice;
	//デバイスコンテキスト
	ComPtr<ID2D1DeviceContext> mD2DDeviceContext;
	//ファクトリー
	ComPtr<ID2D1Factory1> mD2DFactory;
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
	Base3DConstBuf* CB0Data;
	ComPtr<ID3D12Resource> ConstBuf0;
	ComPtr<ID3D12DescriptorHeap> SharedCb0vHeap;

	//2D描画
	ComPtr<ID3D11On12Device> mD3D11On12Device;
	ComPtr<ID3D11DeviceContext> mD3D11DeviceContext;
	ComPtr<IDWriteFactory> mDWriteFactory;
	ComPtr<ID2D1Bitmap1> mD2DRenderTargets[2];
	ComPtr<ID3D11Resource> mWrappedBackBuffers[2];

};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);