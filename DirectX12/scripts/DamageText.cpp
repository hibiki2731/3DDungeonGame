#include "DamageText.h"
#include "Game.h"



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

DamageTextManager::DamageTextManager(const std::shared_ptr<Game>& game)
{
	mGame = game;

	////vertexBuffer作成
	mVertexRawData.reserve(SizeInByte);
	HRESULT hr = mGame->getGraphic()->createBuf(SizeInByte, mVertexBuf);
	assert(SUCCEEDED(hr));

	////vertexBufferView作成
	mVertexBufView.BufferLocation = mVertexBuf->GetGPUVirtualAddress();
	mVertexBufView.SizeInBytes = SizeInByte;
	mVertexBufView.StrideInBytes = sizeof(float) * NumElementsPerVertex; //頂点ごとのバイト数

	////ビルボード処理用のコンスタントバッファを作成
	hr = mGame->getGraphic()->createBuf(mGame->getGraphic()->alignedSize(sizeof(BillboardConstBuf)), mBillboardConstBuf);
	assert(SUCCEEDED(hr));
	////コンスタントバッファの初期化
	hr = mGame->getGraphic()->mapBuf((void**)&mBC, mBillboardConstBuf);
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, mGame->getGraphic()->getAspect(), 0.01f, 50.0f);
	mBC->proj = proj;

	////ファイルを読み込み、テクスチャバッファをつくる
	hr = mGame->getGraphic()->createShaderResource("assets\\digits.png", mTextureBuf);
	assert(SUCCEEDED(hr));

	////DescripterHeapの作成
	hr = mGame->getGraphic()->createCbvTbvHeap(mDescHeap, 2);
	assert(SUCCEEDED(hr));

	////ディスクリプタヒープにビューを作成
	auto hDescHeap = mDescHeap->GetCPUDescriptorHandleForHeapStart();
	UINT cbvTbvSize = mGame->getGraphic()->getCbvTbvIncSize();
	mGame->getGraphic()->createConstantBufferView(mBillboardConstBuf, hDescHeap); 
	hDescHeap.ptr += cbvTbvSize;
	mGame->getGraphic()->createShaderResourceView(mTextureBuf, hDescHeap);		

}

DamageTextManager::~DamageTextManager()
{
	mBillboardConstBuf->Unmap(0, nullptr);
}

void DamageTextManager::update()
{
	std::vector<std::shared_ptr<DamageText>> deadTexts;

	int t = 0;
	for (auto& damageText : mDamageTexts) {
		damageText->update();

		mVertexRawData[t * 6] = damageText->getPosX();
		mVertexRawData[t * 6 + 1] = damageText->getPosY();
		mVertexRawData[t * 6 + 2] = damageText->getPosZ();
		mVertexRawData[t * 6  + 5] = damageText->getLifeTime() / MaxLifeTime;

		//寿命が切れたら削除待ち配列に追加
		if (damageText->getLifeTime() <= 0.0f) deadTexts.push_back(damageText);

		t++;
	}

	//VertexBufferを更新
	mGame->getGraphic()->updateBuf(mVertexRawData.data(), SizeInByte, mVertexBuf);

	//削除待ち配列中の要素を削除
	for (auto damageText : deadTexts) {

		//mDamageTextsから削除
		auto iter = std::find(mDamageTexts.begin(), mDamageTexts.end(), damageText);
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
	mGame->getGraphic()->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//ディスクリプタヒープをGPUにセット
	UINT numDescriptorHeaps = 1;
	mGame->getGraphic()->getCommandList()->SetDescriptorHeaps(numDescriptorHeaps, mDescHeap.GetAddressOf());
	//パーツごとに描画
	//頂点をセット
	mGame->getGraphic()->getCommandList()->IASetVertexBuffers(0, 1, &mVertexBufView);

	//ディスクリプタヒープをディスクリプタテーブルにセット
	auto hDescHeap = mDescHeap->GetGPUDescriptorHandleForHeapStart();
	mGame->getGraphic()->getCommandList()->SetGraphicsRootDescriptorTable(0, hDescHeap);
	//描画。インデックスを使用しない
	mGame->getGraphic()->getCommandList()->DrawInstanced(1, mDamageTexts.size(), 0, 0);
	
}

void DamageTextManager::createDamageText(XMFLOAT3& position, int digit)
{
	std::shared_ptr<DamageText> damageText = std::make_shared<DamageText>(position, digit, MaxLifeTime, Velocity);
	if(mDamageTexts.size() == MaxNum){
		//mDamageTexts配列が埋まっている場合、先頭に要素を追加
		//damageTextを配列に追加
		mDamageTexts[0] = damageText;
		//生データを配列に追加
		mVertexRawData[0] = position.x;
		mVertexRawData[1] = position.y;
		mVertexRawData[2] = position.z;
		mVertexRawData[3] = DTSize;
		mVertexRawData[4] = (float)digit;
		mVertexRawData[5] = 1.0f; //初期値は1
	}
	else {
		//damageTextを配列に追加
		mDamageTexts.push_back(damageText);
		//生データを配列に追加
		mVertexRawData.push_back(position.x);
		mVertexRawData.push_back(position.y);
		mVertexRawData.push_back(position.z);
		mVertexRawData.push_back(DTSize);
		mVertexRawData.push_back((float)digit);
		mVertexRawData.push_back(1.0f); //初期値は1
	}
}

void DamageTextManager::updateView(XMMATRIX& view)
{
	mBC->view = view;
}

float DamageTextManager::getSize()
{
	return DTSize;
}
