#include "Anime2DComponent.h"
#include "Actor.h"
#include <string>


float anime2DVertices[] = {
	-1.0f, 1.0f, 0.0f, 0.0f,
	-1.0f,-1.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, 0.0f,
	 1.0f,-1.0f, 1.0f, 1.0f
};

UINT16 anime2DIndices[] = {
	0, 1, 2,
	2, 1, 3
};

void Anime2DComponent::initComponent()
{
	SpriteComponent::initComponent();
	mTextureIndex = 0;
}

void Anime2DComponent::create(const std::string filename, int textureNum)
{
	mTextureNum = textureNum;
	{
		//頂点バッファの作成
		UINT sizeInByte = sizeof(float) * 16;
		Hr = mGraphic->createBuf(sizeInByte, VertexBuf);
		assert(SUCCEEDED(Hr));

		//頂点バッファに生データをコピー
		Hr = mGraphic->updateBuf(anime2DVertices, sizeInByte, VertexBuf);
		assert(SUCCEEDED(Hr));

		//位置バッファのビューを初期化しておく。（ディスクリプタヒープに作らなくてよい）
		VertexBufView.BufferLocation = VertexBuf->GetGPUVirtualAddress();
		VertexBufView.SizeInBytes = sizeInByte;//全バイト数
		VertexBufView.StrideInBytes = sizeof(float) * 4;//１頂点のバイト数
	}
	{
		//インデックスバッファの作成
		UINT sizeInByte = sizeof(UINT16) * 6;
		Hr = mGraphic->createBuf(sizeInByte, IndexBuf);
		assert(SUCCEEDED(Hr));

		//インデックスバッファ人生データをコピー
		Hr = mGraphic->updateBuf(anime2DIndices, sizeInByte, IndexBuf);
		assert(SUCCEEDED(Hr));

		//インデックスバッファービューを作る
		IndexBufView.BufferLocation = IndexBuf->GetGPUVirtualAddress();
		IndexBufView.SizeInBytes = sizeInByte;//全バイト数
		IndexBufView.Format = DXGI_FORMAT_R16_UINT;//UINT16
	}
	//コンスタントバッファ1(World Matrix)
	{
		//コンスタントバッファ1をつくる
		mGraphic->createBuf(mGraphic->alignedSize(sizeof(Cb3)), ConstBuf3);
		mGraphic->mapBuf((void**)&Cb3, ConstBuf3);
	}
	//シェーダーリソース
	{
		int dotPos = filename.rfind('.');
		std::string preName = filename.substr(0, dotPos);
		std::string postName = filename.substr(dotPos);

		for (int i = 0;i < mTextureNum;i++)
		{
			ComPtr<ID3D12Resource> textureBuf;
			std::string textureName = preName + std::to_string(i) + postName;
			Hr = mGraphic->createShaderResource(textureName, textureBuf);
			TextureBufs.emplace_back(textureBuf);
			assert(SUCCEEDED(Hr));
		}
	}
	//ディスクリプタヒープを作る
	{
		Hr = mGraphic->createCbvTbvHeap(CbvTbvHeap, 1 + mTextureNum);
		assert(SUCCEEDED(Hr));

		auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		CbvTbvSize = mGraphic->getCbvTbvIncSize();
		mGraphic->createConstantBufferView(ConstBuf3, hCbvTbvHeap);			hCbvTbvHeap.ptr += CbvTbvSize;
		for (int i = 0; i < mTextureNum; i++) {
			mGraphic->createShaderResourceView(TextureBufs[i], hCbvTbvHeap);		hCbvTbvHeap.ptr += CbvTbvSize;
		}
	}
}

void Anime2DComponent::draw()
{
	//ワールドマトリックス
	XMMATRIX world = XMMatrixIdentity()
		* XMMatrixRotationZ(mOwner->getRotation().z)
		* XMMatrixTranslation(mOwner->getPosition().x, mOwner->getPosition().y, mOwner->getPosition().z)
		* XMMatrixScaling(mOwner->getScale().x, mOwner->getScale().y, mOwner->getScale().z)
		;
	Cb3->world = world;

	Cb3->rect = mRect;

	//ディスクリプタヒープをＧＰＵにセット
	UINT numDescriptorHeaps = 1;
	mCommandList->SetDescriptorHeaps(numDescriptorHeaps, CbvTbvHeap.GetAddressOf());
	//パーツごとに描画
	//頂点をセット
	mCommandList->IASetVertexBuffers(0, 1, &VertexBufView);

	//ディスクリプタヒープをディスクリプタテーブルにセット
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	UINT CbvTbvSize = mGraphic->getCbvTbvIncSize();
	mCommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	hCbvTbvHeap.ptr += CbvTbvSize * (1 + mTextureIndex);
	mCommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
	//描画。インデックスを使用
	mCommandList->IASetIndexBuffer(&IndexBufView);
	mCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Anime2DComponent::setTextureIndex(int index)
{
	if (index < 0) index = 0;
	if (index >= mTextureNum) index = mTextureNum - 1;
	mTextureIndex = index;
}
