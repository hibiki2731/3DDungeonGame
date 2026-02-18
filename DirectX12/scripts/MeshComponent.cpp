#include "MeshComponent.h"
#include<fstream>
#include<vector>
#include "Graphic.h"
#include "Actor.h"
#include "Game.h"



MeshComponent::~MeshComponent()
{
	delete[] Parts;
}

void MeshComponent::initComponent() {
	mGraphic = mOwner->getGame()->getGraphic();
	mCommandList = mGraphic->getCommandList();
	mOwner->getGame()->addMesh(dynamic_pointer_cast<MeshComponent>(shared_from_this()));
	CbvTbvSize = mGraphic->getCbvTbvIncSize();
	NumDescriptors = 4; 
}

void MeshComponent::endProccess()
{
	//Gameからメッシュを削除
	mOwner->getGame()->removeMesh(dynamic_pointer_cast<MeshComponent>(shared_from_this()));
}

void MeshComponent::create(ObjectName objectName)
{	

	//メッシュデータの取得
	std::shared_ptr<MeshData> meshData = mOwner->getGame()->getAssetManager()->getMeshData(objectName);
	
	//コンスタントバッファのインデックスを取得
	mCBIndex = mOwner->getGame()->getAssetManager()->getCBEndIndex();
	mCBSize = (meshData->NumParts + 1) * 256;
	mHeapIndex = mOwner->getGame()->getAssetManager()->getHeapEndIndex();
	mHeapSize = NumDescriptors * meshData->NumParts;

	//メッシュパーツ数を読み込み、メモリを確保
	NumParts = meshData->NumParts;
	Parts = new PARTS[NumParts];

	//フラッシュ用バッファの初期化
	Cb1.flashColor = XMFLOAT3(1.0f, 1.0f, 1.0f);	//白く光る
	Cb1.flashIntensity = 0.0f;				//最初は光らない

	//パーツごとに各バッファをつくる
	for (int k = 0; k < NumParts; k++) {
		//頂点バッファ
		{
			Parts[k].NumVertices = meshData->NumVertices[k];
			Parts[k].VertexBufView = meshData->VertexBufView[k];
		}
		//マテリアル用コンスタントバッファ
		{
			Parts[k].Cb2.ambient = meshData->Material[k * 3];
			Parts[k].Cb2.diffuse = meshData->Material[k * 3 + 1];
			Parts[k].Cb2.specular = meshData->Material[k * 3 + 2];
		}
		//テクスチャバッファ
		{
			//ファイルを読み込み、テクスチャバッファをつくる
			Hr = mGraphic->createShaderResource(meshData->TextureName[k], Parts[k].TextureBuf);
			assert(SUCCEEDED(Hr));
			
		}
	}

	//コンスタントバッファへデータをコピー
	for (int i = 0; i < NumParts; i++) memcpy(mGraphic->getConstantData() + mCBIndex + 256 * (i + 1), &Parts[i].Cb2, sizeof(MaterialConstBuf));


	int heapIndex = mHeapIndex;
	//コンスタントバッファビューを作成
	for (int k = 0; k < NumParts; k++) {
		mGraphic->createBase3DBufferView(heapIndex); heapIndex++;
		mGraphic->createConstantBufferView(mCBIndex, 256, heapIndex); heapIndex++;
		mGraphic->createConstantBufferView(mCBIndex + 256 * (k + 1), 256, heapIndex); heapIndex++;
		mGraphic->createShaderResourceView(Parts[k].TextureBuf, heapIndex); heapIndex++;
	}

	//使用したメモリ分インスタンスを進める
	mOwner->getGame()->getAssetManager()->proceedCBIndex(mCBSize);
	mOwner->getGame()->getAssetManager()->proceedHeapIndex(mHeapSize);

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
	Cb1.world = world;

	memcpy(mGraphic->getConstantData() + mCBIndex, &Cb1, sizeof(World3DConstBuf));

	auto hCbvTbvHeap = mGraphic->getHeapHandle();
	hCbvTbvHeap.ptr += CbvTbvSize * mHeapIndex;

	//パーツごとに描画
	for (int k = 0; k < NumParts; ++k)
	{
		//頂点をセット
		mCommandList->IASetVertexBuffers(0, 1, &Parts[k].VertexBufView);

		//ディスクリプタヒープをディスクリプタテーブルにセット
		mCommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		hCbvTbvHeap.ptr += CbvTbvSize * NumDescriptors;

		//描画。インデックスを使用しない
		mCommandList->DrawInstanced(Parts[k].NumVertices, 1, 0, 0);

	}
}

void MeshComponent::updateFlashIntensity(float intensity)
{
	Cb1.flashIntensity = intensity;
}
