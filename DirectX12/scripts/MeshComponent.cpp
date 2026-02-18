#include "MeshComponent.h"

#include<fstream>
#include<vector>
#include "Graphic.h"
#include "Actor.h"
#include "Game.h"



MeshComponent::~MeshComponent()
{
	for (int k = 0; k < NumParts; k++) {
		Parts[k].ConstBuf2->Unmap(0, nullptr);
	}
	delete[] Parts;
	ConstBuf1->Unmap(0, nullptr);
}

void MeshComponent::initComponent() {
	mGraphic = mOwner->getGame()->getGraphic();
	mCommandList = mGraphic->getCommandList();
	mOwner->getGame()->addMesh(dynamic_pointer_cast<MeshComponent>(shared_from_this()));
}

void MeshComponent::endProccess()
{
	//Gameからメッシュを削除
	mOwner->getGame()->removeMesh(dynamic_pointer_cast<MeshComponent>(shared_from_this()));
}

void MeshComponent::create(ObjectName objectName)
{	

	//world matrix用コンスタントバッファ1をつくる
	mGraphic->createBuf(mGraphic->alignedSize(sizeof(Cb1)), ConstBuf1);
	mGraphic->mapBuf((void**)&Cb1, ConstBuf1);
	//このメッシュの全パーツで共有するコピー元ディスクリプタヒープをつくる
	mGraphic->createSharedCbvTbvHeap(Cb1vHeap, 1);
	mGraphic->createConstantBufferView(ConstBuf1, Cb1vHeap->GetCPUDescriptorHandleForHeapStart());

	//メッシュデータの取得
	std::shared_ptr<MeshData> meshData = mOwner->getGame()->getAssetManager()->getMeshData(objectName);
	
	//メッシュパーツ数を読み込み、メモリを確保
	NumParts = meshData->NumParts;
	Parts = new PARTS[NumParts];

	//パーツごとに各バッファをつくる
	for (int k = 0; k < NumParts; k++) {
		//頂点バッファ
		{
			Parts[k].NumVertices = meshData->NumVertices[k];
			Parts[k].VertexBufView = meshData->VertexBufView[k];
		}
		//マテリアル用コンスタントバッファ
		{
			//コンスタントバッファをつくる
			Hr = mGraphic->createBuf(mGraphic->alignedSize(sizeof(MaterialConstBuf)), Parts[k].ConstBuf2);
			assert(SUCCEEDED(Hr));

			//マップして更新。unmapしない。
			Hr = mGraphic->mapBuf((void**)&Parts[k].Cb2, Parts[k].ConstBuf2);
			assert(SUCCEEDED(Hr));
			Parts[k].Cb2->ambient = meshData->Material[k * 3];
			Parts[k].Cb2->diffuse = meshData->Material[k * 3 + 1];
			Parts[k].Cb2->specular = meshData->Material[k * 3 + 2];

			Parts[k].Cb2->flashColor = XMFLOAT3(1.0f, 1.0f, 1.0f);	//白く光る
			Parts[k].Cb2->flashIntensity = 0.0f;					//最初は光らない
		}
		//テクスチャバッファ
		{
			//ファイルを読み込み、テクスチャバッファをつくる
			Hr = mGraphic->createShaderResource(meshData->TextureName[k], Parts[k].TextureBuf);
			assert(SUCCEEDED(Hr));
			
		}
	}

	//NumParts分の「ディスクリプタヒープ」をつくる
	NumDescriptors = 4;//ひとつのパーツで使用するディスクリプタの数
	Hr = mGraphic->createCbvTbvHeap(CbvTbvHeap, NumDescriptors * NumParts);
	assert(SUCCEEDED(Hr));
	//バッファの「ビュー」を「ディスクリプタヒープ」にならべてコピーしたり、つくったりする
	auto hCbvTbvHeap = CbvTbvHeap->GetCPUDescriptorHandleForHeapStart();
	CbvTbvSize = mGraphic->getCbvTbvIncSize();
	for (int k = 0; k < NumParts; k++) {
		mGraphic->copySharedCbvTbvHeap(mGraphic->getCb0vHeap(), hCbvTbvHeap);	hCbvTbvHeap.ptr += CbvTbvSize;
		mGraphic->copySharedCbvTbvHeap(Cb1vHeap, hCbvTbvHeap);		hCbvTbvHeap.ptr += CbvTbvSize;
		mGraphic->createConstantBufferView(Parts[k].ConstBuf2, hCbvTbvHeap);			hCbvTbvHeap.ptr += CbvTbvSize;
		mGraphic->createShaderResourceView(Parts[k].TextureBuf, hCbvTbvHeap);		hCbvTbvHeap.ptr += CbvTbvSize;
	}

}

void MeshComponent::draw()
{
	//ワールドマトリックス
	XMMATRIX world = XMMatrixIdentity()
		* XMMatrixScaling(mOwner->getScale().x, mOwner->getScale().y, mOwner->getScale().z)
		* XMMatrixRotationX(mOwner->getRotation().x)
		* XMMatrixRotationY(mOwner->getRotation().y)
		* XMMatrixRotationZ(mOwner->getRotation().z)
		* XMMatrixTranslation(mOwner->getPosition().x, mOwner->getPosition().y, mOwner->getPosition().z)
		;
	Cb1->world = world;


	//ディスクリプタヒープをＧＰＵにセット
	UINT numDescriptorHeaps = 1;
	mCommandList->SetDescriptorHeaps(numDescriptorHeaps, CbvTbvHeap.GetAddressOf());
	//パーツごとに描画
	for (int k = 0; k < NumParts; ++k)
	{
		//頂点をセット
		mCommandList->IASetVertexBuffers(0, 1, &Parts[k].VertexBufView);

		//ディスクリプタヒープをディスクリプタテーブルにセット
		auto hCbvTbvHeap = CbvTbvHeap->GetGPUDescriptorHandleForHeapStart();
		hCbvTbvHeap.ptr += CbvTbvSize * NumDescriptors * k;
		mCommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		//描画。インデックスを使用しない
		mCommandList->DrawInstanced(Parts[k].NumVertices, 1, 0, 0);
	}
}

void MeshComponent::updateFlashIntensity(float intensity)
{
	for (int k = 0; k < NumParts; k++) {
		Parts[k].Cb2->flashIntensity = intensity;
	}
}
