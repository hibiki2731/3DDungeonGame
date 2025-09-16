#include "WinMain.h"

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

const char* TextureFilename = "assets\\penguin1.png";

UINT CbvTbvIdx;

INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT) {
	
	Graphic graphic;
	graphic.init();
	graphic.clearColor(0.25f, 0.5f, 0.9f);

	//デバック用
	HRESULT hr;

	//必要な数のCbv、Tbv用のディスクリプタヒープを作成
	{
		hr = graphic.createDescriptorHeap(3);
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
		//コンスタントバッファ0
		{
			//バッファを作る
			hr = graphic.createBuf(graphic.alignedSize(sizeof(CONST_BUF0)), ConstBuf0);
			assert(SUCCEEDED(hr));
			//マップする
			hr = graphic.mapBuf((void**)&MappedConstBuf0, ConstBuf0);
			assert(SUCCEEDED(hr));
			//ビューを作り、インデックスを取得
			CbvTbvIdx = graphic.createConstantBufferView(ConstBuf0);
		}
		//コンスタントバッファ1
		{
			//バッファを作る
			hr = graphic.createBuf(graphic.alignedSize(sizeof(CONST_BUF1)), ConstBuf1);
			assert(SUCCEEDED(hr));
			//マップする
			hr = graphic.mapBuf((void**)&MappedConstBuf1, ConstBuf1);
			assert(SUCCEEDED(hr));
			//データを入れる
			MappedConstBuf1->diffuse = { Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3] };
			//ビューを作る
			graphic.createConstantBufferView(ConstBuf1);
		}
		//テクスチャバッファ
		{
			//バッファを作る
			hr = graphic.createShaderResource(TextureFilename, TextureBuf);
			assert(SUCCEEDED(hr));
			//ビューを作る
			graphic.createShaderResourceView(TextureBuf);
		}
	}


	//メインループ
	while (!graphic.quit()) {

		//更新
		static float r = 0;
		r += 0.01f;
		//ワールドマトリックス
		XMMATRIX world = XMMatrixRotationY(r);
		//ビューマトリックス
		XMFLOAT3 eye = { 0, 0, -2 }, focus = {0, 0, 0}, up{0, 1, 0};
		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, graphic.getAspect(), 1.0f, 10.0f);
		MappedConstBuf0->mat = world * view * proj;

		//描画
		graphic.beginRender();
		graphic.drawMesh(VertexBufView, IndexBufView, CbvTbvIdx);
		graphic.endRender();
	}

	//終了処理
	{
		graphic.waitGPU();
		graphic.closeEventHandle();
		graphic.unmapBuf(ConstBuf0);
		graphic.unmapBuf(ConstBuf1);
	}
	return graphic.msg_wparam();	
}