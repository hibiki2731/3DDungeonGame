#include "WinMain.h"


//頂点位置バッファ
ComPtr<ID3D12Resource> VertexBuf;
D3D12_VERTEX_BUFFER_VIEW VertexBufView;

//頂点インデックスバッファ
ComPtr<ID3D12Resource> IndexBuf;
D3D12_INDEX_BUFFER_VIEW IndexBufView;

//コンスタントバッファ
CONST_BUF0* MappedConstBuf0_a;
ComPtr<ID3D12Resource> ConstBuf0_a;
CONST_BUF1* MappedConstBuf1_a;
ComPtr<ID3D12Resource> ConstBuf1_a;
ComPtr<ID3D12Resource> TextureBuf_a;
CONST_BUF0* MappedConstBuf0_b;
ComPtr<ID3D12Resource> ConstBuf0_b;
CONST_BUF1* MappedConstBuf1_b;
ComPtr<ID3D12Resource> ConstBuf1_b;
ComPtr<ID3D12Resource> TextureBuf_b;


Vertex vertices[] = {
	{XMFLOAT3(-0.5,  0.5,  0.0), XMFLOAT2(0.0f, 0.0f)},
	{XMFLOAT3(-0.5,  -0.5,  0.0), XMFLOAT2(0.0f, 1.0f)},
	{XMFLOAT3(0.5,  0.5,  0.0), XMFLOAT2(1.0f, 0.0f)},
	{XMFLOAT3(0.5,  -0.5,  0.0), XMFLOAT2(1.0f, 1.0f)},
};
//
//float Vertices[] = {
//	//position            texcoord
//	-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, //左上
//	-0.5f, -0.5f,  0.0f,  0.0f,  1.0f, //左下
//	 0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //右上
//	 0.5f, -0.5f,  0.0f,  1.0f,  1.0f, //右下
//};

unsigned short indices[] = {
	0,1,2,
	2,1,3,
};

float Diffuse[] = { 1.0f, 0.5f, 1.0f, 1.0f };

const char* TextureFilename_a = "assets\\penguin1.png";
const char* TextureFilename_b = "assets\\penguin2.png";

UINT CbvTbvIdx;

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT) {
	
	Graphic graphic;
	graphic.init();
	graphic.clearColor(0.25f, 0.5f, 0.9f);

	//デバック用
	HRESULT hr;

	//必要な数のCbv、Tbv用のディスクリプタヒープを作成
	{
		hr = graphic.createDescriptorHeap(6);
		assert(SUCCEEDED(hr));
	}

	//メッシュを一つ描画
	{
		//頂点バッファ
		{
			//データサイズをもとめる
			UINT sizeInBytes = sizeof(vertices);
			UINT strideInBytes = sizeof(vertices[0]);
			//バッファを作る
			hr = graphic.createBuf(sizeInBytes, VertexBuf);
			assert(SUCCEEDED(hr));
			//バッファにデータを入れる
			hr = graphic.updateBuf(vertices, sizeInBytes, VertexBuf);
			assert(SUCCEEDED(hr));
			//ビューを作成
			graphic.createVertexBufferView(VertexBuf, sizeInBytes, strideInBytes, VertexBufView);
		}
		//インデックスバッファ
		{
			//データサイズをもとめる
			UINT sizeInBytes = sizeof(indices);
			//バッファを作る
			hr = graphic.createBuf(sizeInBytes, IndexBuf);
			assert(SUCCEEDED(hr));
			//バッファにデータを入れる
			hr = graphic.updateBuf(indices, sizeInBytes, IndexBuf);
			assert(SUCCEEDED(hr));
			//ビューを作成
			graphic.createIndexBufferView(IndexBuf, sizeInBytes, IndexBufView);
		}
		//コンスタントバッファ0_a
		{
			//バッファを作る
			hr = graphic.createBuf(graphic.alignedSize(sizeof(CONST_BUF0)), ConstBuf0_a);
			assert(SUCCEEDED(hr));
			//マップする
			hr = graphic.mapBuf((void**)&MappedConstBuf0_a, ConstBuf0_a);
			assert(SUCCEEDED(hr));
			//ビューを作り、インデックスを取得
			CbvTbvIdx = graphic.createConstantBufferView(ConstBuf0_a);
		}
		//コンスタントバッファ1_a
		{
			//バッファを作る
			hr = graphic.createBuf(graphic.alignedSize(sizeof(CONST_BUF1)), ConstBuf1_a);
			assert(SUCCEEDED(hr));
			//マップする
			hr = graphic.mapBuf((void**)&MappedConstBuf1_a, ConstBuf1_a);
			assert(SUCCEEDED(hr));
			//データを入れる
			MappedConstBuf1_a->diffuse = { Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3] };
			//ビューを作る
			graphic.createConstantBufferView(ConstBuf1_a);
		}
		//テクスチャバッファ_a
		{
			//バッファを作る
			hr = graphic.createShaderResource(TextureFilename_a, TextureBuf_a);
			assert(SUCCEEDED(hr));
			//ビューを作る
			graphic.createShaderResourceView(TextureBuf_a);
		}
		//コンスタントバッファ0_b
		{
			//バッファを作る
			hr = graphic.createBuf(graphic.alignedSize(sizeof(CONST_BUF0)), ConstBuf0_b);
			assert(SUCCEEDED(hr));
			//マップする
			hr = graphic.mapBuf((void**)&MappedConstBuf0_b, ConstBuf0_b);
			assert(SUCCEEDED(hr));
			//ビューを作り、インデックスを取得
			graphic.createConstantBufferView(ConstBuf0_b);
		}
		//コンスタントバッファ1_b
		{
			//バッファを作る
			hr = graphic.createBuf(graphic.alignedSize(sizeof(CONST_BUF1)), ConstBuf1_b);
			assert(SUCCEEDED(hr));
			//マップする
			hr = graphic.mapBuf((void**)&MappedConstBuf1_b, ConstBuf1_b);
			assert(SUCCEEDED(hr));
			//データを入れる
			MappedConstBuf1_b->diffuse = { Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3] };
			//ビューを作る
			graphic.createConstantBufferView(ConstBuf1_b);
		}
		//テクスチャバッファ_b
		{
			//バッファを作る
			hr = graphic.createShaderResource(TextureFilename_b, TextureBuf_b);
			assert(SUCCEEDED(hr));
			//ビューを作る
			graphic.createShaderResourceView(TextureBuf_b);
		}
	}


	//メインループ
	while (!graphic.quit()) {

		//更新
		static float r = 0;
		r += 0.01f;
		//メッシュa
		{
			//ワールドマトリックス
			XMMATRIX world = XMMatrixTranslation(-1.0f, 0.0f, 0.0f);
			world = XMMatrixRotationY(r) * world;
			//ビューマトリックス
			XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up{ 0, 1, 0 };
			XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
			//プロジェクションマトリックス
			XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, graphic.getAspect(), 1.0f, 10.0f);
			MappedConstBuf0_a->mat = world * view * proj;
		}

		//メッシュb
		{
			//ワールドマトリックス
			XMMATRIX world = XMMatrixTranslation(1.0f, 0.0f, 0.0f);
			world = XMMatrixRotationY(r) * world;
			//ビューマトリックス
			XMFLOAT3 eye = { 0, 0, -2 }, focus = { 0, 0, 0 }, up{ 0, 1, 0 };
			XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
			//プロジェクションマトリックス
			XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, graphic.getAspect(), 1.0f, 10.0f);
			MappedConstBuf0_b->mat = world * view * proj;
		}

		
	
		//描画
		graphic.beginRender();
		graphic.drawMesh(VertexBufView, IndexBufView, CbvTbvIdx);
		CbvTbvIdx += 3;
		graphic.drawMesh(VertexBufView, IndexBufView, CbvTbvIdx);
		graphic.endRender();

		CbvTbvIdx = 0;
	}

	//終了処理
	{
		graphic.waitGPU();
		graphic.closeEventHandle();
		graphic.unmapBuf(ConstBuf0_a);
		graphic.unmapBuf(ConstBuf1_a);
	}
	return graphic.msg_wparam();	
}