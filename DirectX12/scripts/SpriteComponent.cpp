#include "SpriteComponent.h"

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "Graphic.h"
#include "Actor.h"
#include "Game.h"

float spriteVertices[] = {
	-1.0f, 1.0f, 0.0f, 0.0f,
	-1.0f,-1.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, 0.0f,
	 1.0f,-1.0f, 1.0f, 1.0f
};

UINT16 spriteIndices[] = {
	0, 1, 2,
	2, 1, 3
};

SpriteComponent::~SpriteComponent()
{
	ConstBuf3->Unmap(0, nullptr);
}

void SpriteComponent::initComponent()
{
	mRect = { 0.0f, 0.0f, 1.0f, 1.0f };

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
		UINT sizeInByte = sizeof(float) * 16;
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
		UINT sizeInByte = sizeof(UINT16) * 6;
		Hr = mGraphic->createBuf(sizeInByte, IndexBuf);
		assert(SUCCEEDED(Hr));

		//インデックスバッファ人生データをコピー
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
		Hr = mGraphic->createShaderResource(filename, TextureBuf);
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

}

void SpriteComponent::draw()
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
	mCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	
}

void SpriteComponent::setRect(const XMFLOAT4& rect)
{
	mRect = rect;
}
