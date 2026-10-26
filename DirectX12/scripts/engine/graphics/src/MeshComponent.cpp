#include "MeshComponent.h"
#include<fstream>
#include<vector>
#include "Graphic.h"
#include "Actor.h"
#include "Scene.h"
#include "Game.h"
#include "myJson.h"
#include "Benchmark.h"

MeshComponent::MeshComponent(Actor& owner, int updateOrder) 
	: Component(owner, updateOrder),
	mGraphic(owner.getScene().getGame().getGraphic())
{
	mCommandList = mGraphic.getCommandList();
	mOwner.getScene().addMesh(this);
	CbvTbvSize = mGraphic.getCbvTbvIncSize();
	mMeshID = "NONE";
}

void MeshComponent::loadFromJson(const nlohmann::json& json)
{
	std::string meshID = json.value("meshID", "GRASS");
	create(meshID);
}

void MeshComponent::endProcess()
{
	//Gameからメッシュを削除
	mOwner.getScene().removeMesh(this);
	//計測用(段階1,2): 個別に作成したリソースを解放
	if (Bench::stage() <= 2) {
		releaseOwnResources();
		return;
	}
	//メッシュの生成に失敗している場合、メモリは確保されていないので解放しない
	if (mCBIndex < 0 || mHeapIndex < 0) return;
	mOwner.getScene().getGame().getAssetManager().deleteMemory(mCBIndex, mCBSize);
	mOwner.getScene().getGame().getAssetManager().deleteHeap(mHeapIndex, mHeapSize * 2);
}

void MeshComponent::create(const std::string& meshID)
{
	MeshData* meshData = nullptr;
	if (Bench::stage() == 1) {
		//計測用(段階1): キャッシュを使わず、オブジェクトごとに頂点バッファとテクスチャを作成
		mOwnedMeshData = mOwner.getScene().getGame().getAssetManager().createUncachedMeshData(meshID);
		meshData = mOwnedMeshData.get();
	}
	else {
		meshData = mOwner.getScene().getGame().getAssetManager().getMeshData(meshID);
	}
	if (meshData == nullptr) return;

	mMeshID = meshID;
	create(meshData);
}

void MeshComponent::create(const MeshData * meshData)
{	
	if (meshData == nullptr) return;

	//計測用(段階1,2): アロケータを使わず、リソースを個別に作成する
	if (Bench::stage() <= 2) {
		NumParts = meshData->NumParts;
		Parts.resize(NumParts);
		Cb1.flashColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
		Cb1.flashIntensity = 0.0f;
		for (int k = 0; k < NumParts; k++) {
			Parts[k].NumVertices = meshData->NumVertices[k];
			Parts[k].VertexBufView = meshData->VertexBufView[k];
			Parts[k].Cb2.ambient = meshData->Material[k * 3];
			Parts[k].Cb2.diffuse = meshData->Material[k * 3 + 1];
			Parts[k].Cb2.specular = meshData->Material[k * 3 + 2];
			Parts[k].TextureBuf = meshData->OwnedTexture.empty()
				? mOwner.getScene().getGame().getAssetManager().getShaderResource(meshData->TextureName[k])
				: meshData->OwnedTexture[k].Get();
		}
		createOwnResources();

		const XMFLOAT3 ownerScale = mOwner.getScale();
		const XMFLOAT3 meshScale = meshData->Scale;
		mOwner.setScale(XMFLOAT3(ownerScale.x * meshScale.x, ownerScale.y * meshScale.y, ownerScale.z * meshScale.z));
		return;
	}

	//コンスタントバッファのインデックスを取得
	mCBSize = (meshData->NumParts + 1) * 256;
	mCBIndex = mOwner.getScene().getGame().getAssetManager().getCBEndIndex(mCBSize);
	mHeapSize = NumDescriptors * meshData->NumParts;
	mHeapIndex = mOwner.getScene().getGame().getAssetManager().getHeapEndIndex(mHeapSize * 2); //二つ分Viewを作る必要がある

	//メッシュパーツ数を読み込み、メモリを確保
	NumParts = meshData->NumParts;
	Parts.resize(NumParts);

	//フラッシュ用バッファの初期化
	Cb1.flashColor = XMFLOAT3(1.0f, 1.0f, 1.0f);	//白く光る
	Cb1.flashIntensity = 0.0f;				//最初は光らない

	//パーツごとに各バッファ情報を取得
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
		{
			Parts[k].TextureBuf = mOwner.getScene().getGame().getAssetManager().getShaderResource(meshData->TextureName[k]);
		}
	}

	//コンスタントバッファへデータをコピー
	for (int i = 0; i < NumParts; i++) {
		memcpy(mGraphic.getConstantData(0) + mCBIndex + 256 * (i + 1), &Parts[i].Cb2, sizeof(MaterialConstBuf));
		memcpy(mGraphic.getConstantData(1) + mCBIndex + 256 * (i + 1), &Parts[i].Cb2, sizeof(MaterialConstBuf));

	}



	int heapIndex = mHeapIndex;
	//コンスタントバッファビューを作成
	for (int k = 0; k < NumParts; k++) {
		mGraphic.createBase3DBufferView(heapIndex, mHeapSize); heapIndex++;
		mGraphic.createConstantBufferView(mCBIndex, 256, heapIndex, mHeapSize); heapIndex++;
		mGraphic.createConstantBufferView(mCBIndex + 256 * (k + 1), 256, heapIndex, mHeapSize); heapIndex++;
		mGraphic.createShaderResourceView(Parts[k].TextureBuf, heapIndex); 
		mGraphic.createShaderResourceView(Parts[k].TextureBuf, heapIndex + mHeapSize); 
		heapIndex++;
	}

	//スケールを設定
	//オーナーのスケール値にメッシュのスケールを掛け合わせる。
	const XMFLOAT3 ownerScale = mOwner.getScale();
	const XMFLOAT3 meshScale = meshData->Scale;
	XMFLOAT3 resultScale = XMFLOAT3(ownerScale.x * meshScale.x, ownerScale.y * meshScale.y, ownerScale.z * meshScale.z);

	mOwner.setScale(resultScale);

}

void MeshComponent::draw()
{
	if (Bench::stage() <= 2) {
		drawOwnResources();
		return;
	}

	//ワールドマトリックス
	XMMATRIX world = XMMatrixIdentity()
		* XMMatrixScaling(mOwner.getScale().x, mOwner.getScale().y, mOwner.getScale().z)
		* XMMatrixRotationX(mOwner.getRotation().x)
		* XMMatrixRotationY(mOwner.getRotation().y)
		* XMMatrixRotationZ(mOwner.getRotation().z)
		* XMMatrixTranslation(mOwner.getPosition().x, mOwner.getPosition().y, mOwner.getPosition().z)
		;
	Cb1.world = world;

	memcpy(mGraphic.getConstantData() + mCBIndex, &Cb1, sizeof(World3DConstBuf));

	auto hCbvTbvHeap = mGraphic.getHeapHandle();
	hCbvTbvHeap.ptr += CbvTbvSize * (mHeapIndex + mGraphic.getBackBufIdx() * mHeapSize);

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

const std::string& MeshComponent::getMeshID()
{
	return mMeshID;
}

//---計測用(段階1,2)---
//アロケータ導入前と同様に、コンスタントバッファ(World + パーツごとのマテリアル)と
//シェーダーから見えるディスクリプタヒープをオブジェクトごとに作成する。
//ダブルバッファリングのため、各コンスタントバッファはフレーム数分の領域を持つ。
void MeshComponent::createOwnResources()
{
	const UINT slotSize = 256;
	const int numCB = 1 + NumParts;
	mOwnCB.resize(numCB);
	mOwnCBData.resize(numCB);
	for (int i = 0; i < numCB; i++) {
		HRESULT hr = mGraphic.createBuf(slotSize * Graphic::FrameCount, mOwnCB[i]);
		assert(SUCCEEDED(hr));
		hr = mGraphic.mapBuf(reinterpret_cast<void**>(&mOwnCBData[i]), mOwnCB[i]);
		assert(SUCCEEDED(hr));
	}
	//マテリアルをコピー
	for (int k = 0; k < NumParts; k++) {
		for (int frame = 0; frame < Graphic::FrameCount; frame++) {
			memcpy(mOwnCBData[k + 1] + slotSize * frame, &Parts[k].Cb2, sizeof(MaterialConstBuf));
		}
	}

	//ヒープの並びは現行と同じ(パーツごとに b0,b1,b2,t0 をフレーム数分)
	mHeapSize = NumDescriptors * NumParts;
	HRESULT hr = mGraphic.createCbvTbvHeap(mOwnHeap, mHeapSize * Graphic::FrameCount);
	assert(SUCCEEDED(hr));

	const UINT base3DSize = Graphic::alignedSize(sizeof(Base3DData));
	for (int frame = 0; frame < Graphic::FrameCount; frame++) {
		int heapIndex = frame * mHeapSize;
		for (int k = 0; k < NumParts; k++) {
			mGraphic.createConstantBufferViewOn(mOwnHeap.Get(), heapIndex++, mGraphic.getBase3DAddress(frame), base3DSize);
			mGraphic.createConstantBufferViewOn(mOwnHeap.Get(), heapIndex++, mOwnCB[0]->GetGPUVirtualAddress() + slotSize * frame, slotSize);
			mGraphic.createConstantBufferViewOn(mOwnHeap.Get(), heapIndex++, mOwnCB[k + 1]->GetGPUVirtualAddress() + slotSize * frame, slotSize);
			mGraphic.createShaderResourceViewOn(mOwnHeap.Get(), heapIndex++, Parts[k].TextureBuf);
		}
	}
}

void MeshComponent::drawOwnResources()
{
	if (!mOwnHeap) return;

	XMMATRIX world = XMMatrixIdentity()
		* XMMatrixScaling(mOwner.getScale().x, mOwner.getScale().y, mOwner.getScale().z)
		* XMMatrixRotationX(mOwner.getRotation().x)
		* XMMatrixRotationY(mOwner.getRotation().y)
		* XMMatrixRotationZ(mOwner.getRotation().z)
		* XMMatrixTranslation(mOwner.getPosition().x, mOwner.getPosition().y, mOwner.getPosition().z)
		;
	Cb1.world = world;

	const int frame = mGraphic.getBackBufIdx();
	memcpy(mOwnCBData[0] + 256 * frame, &Cb1, sizeof(World3DConstBuf));

	//オブジェクトごとのヒープをセット(アロケータ導入前は描画のたびに行っていた)
	mCommandList->SetDescriptorHeaps(1, mOwnHeap.GetAddressOf());

	auto hCbvTbvHeap = mOwnHeap->GetGPUDescriptorHandleForHeapStart();
	hCbvTbvHeap.ptr += CbvTbvSize * (frame * mHeapSize);

	for (int k = 0; k < NumParts; ++k)
	{
		mCommandList->IASetVertexBuffers(0, 1, &Parts[k].VertexBufView);
		mCommandList->SetGraphicsRootDescriptorTable(0, hCbvTbvHeap);
		hCbvTbvHeap.ptr += CbvTbvSize * NumDescriptors;
		mCommandList->DrawInstanced(Parts[k].NumVertices, 1, 0, 0);
	}
}

void MeshComponent::releaseOwnResources()
{
	//GPUが使用中の可能性があるため、遅延削除する
	auto release = [this](auto& resource) {
		if (!resource) return;
		ComPtr<IUnknown> unknown;
		resource.As(&unknown);
		resource.Reset();
		mGraphic.delayRelease(unknown);
	};

	for (auto& cb : mOwnCB) {
		cb->Unmap(0, nullptr);
		release(cb);
	}
	mOwnCB.clear();
	mOwnCBData.clear();
	release(mOwnHeap);

	if (mOwnedMeshData) {
		for (auto& buf : mOwnedMeshData->VertexBuf) release(buf);
		for (auto& tex : mOwnedMeshData->OwnedTexture) release(tex);
		mOwnedMeshData.reset();
	}
}
