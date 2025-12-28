#include "SpriteComponent.h"

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "Graphic.h"
#include "Actor.h"
#include "Game.h"

float spriteVertices[] = {
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.3333f, 0.0f, 0.3333f,
	 0.3333f, 0.0f, 0.3333f, 0.0f,
	 0.3333f, 0.3333f, 0.3333f, 0.3333f,
	 0.6666f, 0.0f, 0.6666f, 0.0f,
	 0.6666f, 0.3333f, 0.6666f, 0.3333f,
	 1.0f, 0.0f, 1.0f, 0.0f,
	 1.0f, 0.3333f, 1.0f, 0.3333f,
	 0.0f,  0.6666f, 0.0f, 0.6666f,
	 0.0f,  1.0f, 0.0f, 1.0f,
	 0.3333f,  0.6666f, 0.3333f, 0.6666f,
	 0.3333f,  1.0f, 0.3333f, 1.0f,
	 0.6666f,  0.6666f, 0.6666f, 0.6666f,
	 0.6666f,  1.0f, 0.6666f, 1.0f,
	 1.0f,  0.6666f, 1.0f, 0.6666f,
	 1.0f,  1.0f, 1.0f, 1.0f,
};

UINT16 spriteIndices[] = {
	0, 1, 2,
	2, 1, 3,
	2, 3, 4,
	4, 3, 5,
	4, 5, 6,
	6, 5, 7,
	1, 8, 3,
	3, 8, 10,
	3, 10, 5,
	5, 10, 12,
	5, 12, 7,
	7, 12, 14,
	8, 9, 10,
	10, 9, 11,
	10, 11, 12,
	12, 11, 13,
	12, 13, 14,
	14, 13, 15,
};

SpriteComponent::~SpriteComponent()
{
	ConstBuf3->Unmap(0, nullptr);
}

void SpriteComponent::initComponent()
{
	mPosition = { 0.0f, 0.0f };
	mScale = { 1.0f, 1.0f };
	mSpriteSize = { 100.0f, 100.0f };
	mBordarSize = 0.0f;

	mGraphic = mOwner->getGame()->getGraphic();
	mCommandList = mGraphic->getCommandList();
	mOwner->getGame()->addSprite(std::dynamic_pointer_cast<SpriteComponent>(shared_from_this()));
}

void SpriteComponent::endProccess()
{
	//Gameからスプライトを削除
	mOwner->getGame()->removeSprite(std::dynamic_pointer_cast<SpriteComponent>(shared_from_this()));
}

void SpriteComponent::create(const std::string filename)
{
	{
		//頂点バッファの作成
		UINT sizeInByte = sizeof(spriteVertices);
		Hr = mGraphic->createBuf(sizeInByte, VertexBuf);
		assert(SUCCEEDED(Hr));

		//頂点バッファに生データをコピー
		Hr = mGraphic->updateBuf(spriteVertices, sizeInByte, VertexBuf);
		assert(SUCCEEDED(Hr));

		//位置バッファのビューを初期化しておく。（ディスクリプタヒープに作らなくてよい）
		VertexBufView.BufferLocation = VertexBuf->GetGPUVirtualAddress();
		VertexBufView.SizeInBytes = sizeInByte;//全バイト数
		VertexBufView.StrideInBytes = sizeof(float) * 4;//１頂点のバイト数
	}
	{
		//インデックスバッファの作成
		UINT sizeInByte = sizeof(spriteIndices);
		Hr = mGraphic->createBuf(sizeInByte, IndexBuf);
		assert(SUCCEEDED(Hr));

		//インデックスバッファに生データをコピー
		Hr = mGraphic->updateBuf(spriteIndices, sizeInByte, IndexBuf);
		assert(SUCCEEDED(Hr));

		//インデックスバッファービューを作る
		IndexBufView.BufferLocation = IndexBuf->GetGPUVirtualAddress();
		IndexBufView.SizeInBytes = sizeInByte;//全バイト数
		IndexBufView.Format = DXGI_FORMAT_R16_UINT;//UINT16
	}
	//コンスタントバッファ(World Matrix)
	{
		//コンスタントバッファをつくる
		mGraphic->createBuf(mGraphic->alignedSize(sizeof(Cb3)), ConstBuf3);
		mGraphic->mapBuf((void**)&Cb3, ConstBuf3);
	}
	//シェーダーリソース
	{
		mTextureSize = mGraphic->createShaderResourceGetSize(filename, TextureBuf);
		assert(SUCCEEDED(Hr));
	}
	//ディスクリプタヒープを作る
	{
		Hr = mGraphic->createCbvTbvHeap(CbvTbvHeap, 2);
		assert(SUCCEEDED(Hr));

		auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
		CbvTbvSize = mGraphic->getCbvTbvIncSize();
		mGraphic->createConstantBufferView(ConstBuf3, hCbvTbvHeap);			hCbvTbvHeap.ptr += CbvTbvSize;
		mGraphic->createShaderResourceView(TextureBuf, hCbvTbvHeap);		hCbvTbvHeap.ptr += CbvTbvSize;
		
	}

	//コンスタントバッファの初期化
	{
		Cb3->windowSize = XMFLOAT2(
			(float)mGraphic->getClientWidth(),
			(float)mGraphic->getClientHeight()
		);
		Cb3->spriteSize = mSpriteSize;
		Cb3->textureSize = mTextureSize;
		Cb3->bordarSize = mBordarSize;
	}

}

void SpriteComponent::draw()
{
	//コンスタントバッファの更新
	//ワールドマトリックス
	XMMATRIX world = XMMatrixIdentity()
		* XMMatrixScaling(mScale.x, mScale.y, 1.0f)
		* XMMatrixRotationZ(mOwner->getRotation().z)
		* XMMatrixTranslation(mPosition.x, mPosition.y, 0.0f)
		;
	Cb3->world = world;
	Cb3->spriteSize = mSpriteSize;	//スプライトサイズ
	Cb3->bordarSize = mBordarSize;	//ボーダーサイズ

	//ディスクリプタヒープをＧＰＵにセット
	UINT numDescriptorHeaps = 1;
	mCommandList->SetDescriptorHeaps(numDescriptorHeaps, CbvTbvHeap.GetAddressOf());

	//頂点をセット
	mCommandList->IASetVertexBuffers(0, 1, &VertexBufView);

	//ディスクリプタヒープをディスクリプタテーブルにセット
	auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
	UINT CbvTbvSize = mGraphic->getCbvTbvIncSize();
	mCommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
	hCbvTbvHeap.ptr += CbvTbvSize;
	mCommandList->SetGraphicsRootDescriptorTable(1, hCbvTbvHeap);
	//描画。インデックスを使用
	mCommandList->IASetIndexBuffer(&IndexBufView);
	mCommandList->DrawIndexedInstanced(std::size(spriteIndices), 1, 0, 0, 0);
	
}

void SpriteComponent::setPosition(const XMFLOAT2& position)
{
	mPosition = position;
}

void SpriteComponent::setScale(const XMFLOAT2& scale)
{
	mScale = scale;
}

void SpriteComponent::setSpriteSize(const XMFLOAT2& size)
{
	mSpriteSize = size;
}

void SpriteComponent::setBordarSize(const float size)
{
	mBordarSize = size;
}
