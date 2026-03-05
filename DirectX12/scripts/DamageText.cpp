#include "DamageText.h"
#include "Game.h"
#include "AssetManager.h"


DamageText::DamageText(XMFLOAT3& position, int digit, float maxLifeTime, XMFLOAT3 velocity)
{
	mCenterPosition = position;
	mVelocity = velocity;
	mDigit = digit;
	mLifeTime = maxLifeTime;

}

DamageText::~DamageText()
{
}

void DamageText::update()
{
	mLifeTime -= deltaTime;	//寿命の更新
	mCenterPosition = mCenterPosition + mVelocity * deltaTime;	//位置の更新
}

float DamageText::getPosX()
{
	return mCenterPosition.x;
}

float DamageText::getPosY() {
	return mCenterPosition.y;
}

float DamageText::getPosZ() {
	return mCenterPosition.z;
}

float DamageText::getLifeTime()
{
	return mLifeTime;
}

DamageTextManager::DamageTextManager(Game* game)
{
	mGame = game;
	mCBSize = 256; //使用するコンスタントバッファは一つだけ
	mHeapSize = 1;
	mCBIndex = mGame->getAssetManager()->getCBEndIndex(mCBSize);
	mHeapIndex = mGame->getAssetManager()->getHeapEndIndex(mHeapSize);

	////vertexBuffer作成
	mVertexRawData.reserve(SizeInByte);
	HRESULT hr;
	for(int i = 0; i < 2; i ++) hr = mGame->getGraphic()->createBuf(SizeInByte, mVertexBuf[i]);
	assert(SUCCEEDED(hr));

	////vertexBufferView作成
	for (int i = 0; i < 2; i++) {
		mVertexBufView[i].BufferLocation = mVertexBuf[i]->GetGPUVirtualAddress();
		mVertexBufView[i].SizeInBytes = SizeInByte;
		mVertexBufView[i].StrideInBytes = sizeof(float) * NumElementsPerVertex; //頂点ごとのバイト数
	}

	//コンスタントバッファの初期化
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, mGame->getGraphic()->getAspect(), 0.01f, 50.0f);
	mBC.proj = proj;
	memcpy(mGame->getGraphic()->getConstantData() + mCBIndex, &mBC, sizeof(BillboardConstBuf));

	////ファイルを読み込み、テクスチャバッファをつくる
	mTextureBuf = mGame->getAssetManager()->getShaderResource("assets\\picture\\digits.png");

	////ディスクリプタヒープにビューを作成
	auto heapIndex = mHeapIndex;
	mGame->getGraphic()->createConstantBufferView(mCBIndex, mCBSize, heapIndex, mHeapSize); heapIndex += 2;
	mGame->getGraphic()->createShaderResourceView(mTextureBuf, heapIndex);

}

DamageTextManager::~DamageTextManager()
{
}

void DamageTextManager::update()
{
	std::vector<DamageText*> deadTexts;

	int t = 0;
	for (auto& damageText : mDamageTexts) {
		//tが範囲外にならないよう
		if (t < 0 || t >= mVertexRawData.size()) continue;

		damageText->update();

		mVertexRawData[t * 6] = damageText->getPosX();
		mVertexRawData[t * 6 + 1] = damageText->getPosY();
		mVertexRawData[t * 6 + 2] = damageText->getPosZ();
		mVertexRawData[t * 6  + 5] = damageText->getLifeTime() / MaxLifeTime;

		//寿命が切れたら削除待ち配列に追加
		if (damageText->getLifeTime() <= 0.0f) { 
			deadTexts.emplace_back(damageText.get()); 
		}

		t++;
	}

	//VertexBufferを更新
	mGame->getGraphic()->updateBuf(mVertexRawData.data(), SizeInByte, mVertexBuf[mGame->getGraphic()->getBackBufIdx()]);

	//削除待ち配列中の要素を削除
	for (auto text : deadTexts) {

		//mDamageTextsから削除
		auto iter = std::find_if(mDamageTexts.begin(), mDamageTexts.end(), [text](const std::unique_ptr<DamageText>& damageText) {
			return text == damageText.get();
			});

		if (iter != mDamageTexts.end()) {

			//生データから削除
			int i = std::distance(mDamageTexts.begin(), iter);
			mVertexRawData[i * NumElementsPerVertex + 5] = mVertexRawData.back(); mVertexRawData.pop_back();
			mVertexRawData[i * NumElementsPerVertex + 4] = mVertexRawData.back(); mVertexRawData.pop_back();
			mVertexRawData[i * NumElementsPerVertex + 3] = mVertexRawData.back(); mVertexRawData.pop_back();
			mVertexRawData[i * NumElementsPerVertex + 2] = mVertexRawData.back(); mVertexRawData.pop_back();
			mVertexRawData[i * NumElementsPerVertex + 1] = mVertexRawData.back(); mVertexRawData.pop_back();
			mVertexRawData[i * NumElementsPerVertex] = mVertexRawData.back(); mVertexRawData.pop_back();

			std::iter_swap(iter, mDamageTexts.end() - 1);
			mDamageTexts.pop_back();

		}
	}
}

void DamageTextManager::draw()
{
	//描画するダメージテキストがなければ、描画処理を抜ける
	if (mDamageTexts.size() == 0)  return; 

	mGame->getGraphic()->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//頂点をセット
	int backBufIdx = mGame->getGraphic()->getBackBufIdx();
	mGame->getGraphic()->getCommandList()->IASetVertexBuffers(0, 1, &mVertexBufView[backBufIdx]);

	//ディスクリプタヒープをディスクリプタテーブルにセット
	auto hDescHeap = mGame->getGraphic()->getHeapHandle();
	hDescHeap.ptr += backBufIdx * mGame->getGraphic()->getCbvTbvIncSize();
	mGame->getGraphic()->getCommandList()->SetGraphicsRootDescriptorTable(0, hDescHeap);
	//描画。インデックスを使用しない
	mGame->getGraphic()->getCommandList()->DrawInstanced(1, mDamageTexts.size(), 0, 0);
	
}

void DamageTextManager::createDamageText(XMFLOAT3& position, int digit)
{
	auto damageText = std::make_unique<DamageText>(position, digit, MaxLifeTime, Velocity);
	if(mDamageTexts.size() == MaxNum){
		//mDamageTexts配列が埋まっている場合、先頭に要素を追加
		//生データを配列に追加
		mVertexRawData[0] = position.x;
		mVertexRawData[1] = position.y;
		mVertexRawData[2] = position.z;
		mVertexRawData[3] = DTSize;
		mVertexRawData[4] = (float)digit;
		mVertexRawData[5] = 1.0f; //初期値は1
		//damageTextを配列に追加
		mDamageTexts[0] = std::move(damageText);
	}
	else {
		//生データを配列に追加
		mVertexRawData.emplace_back(position.x);
		mVertexRawData.emplace_back(position.y);
		mVertexRawData.emplace_back(position.z);
		mVertexRawData.emplace_back(DTSize);
		mVertexRawData.emplace_back((float)digit);
		mVertexRawData.emplace_back(1.0f); //初期値は1
		//damageTextを配列に追加
		mDamageTexts.emplace_back(std::move(damageText));
	}
}

void DamageTextManager::updateView(XMMATRIX& view)
{
	mBC.view = view;
	memcpy(mGame->getGraphic()->getConstantData() + mCBIndex, &mBC, sizeof(BillboardConstBuf));
}

float DamageTextManager::getSize()
{
	return DTSize;
}
