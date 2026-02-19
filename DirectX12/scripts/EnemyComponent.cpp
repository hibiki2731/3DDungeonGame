#include "EnemyComponent.h"
#include "Actor.h"
#include "Game.h"

void EnemyComponent::initComponent()
{
	CharacterComponent::initComponent();
	mOwner->getGame()->addEnemy(dynamic_pointer_cast<EnemyComponent>(shared_from_this()));

	mFlashTimer = 0.0f;
	mFlashDuration = 0.3f;	//ダメージを受けたときの点滅時間

	//乱数生成期の初期化
	unsigned seed = timeGetTime();
	mt.seed(seed);

	isMoving = false;
	mMoveSpeed = 5.0f;

	mEnemyType = ObjectType::EMPTY;
}

void EnemyComponent::inputComponent()
{
	CharacterComponent::inputComponent();
}

void EnemyComponent::updateComponent()
{
	CharacterComponent::updateComponent();

	//点滅処理の更新
	updateFlash();

	//エネミーターン時の動作
	move();

	//移動
	if (isMoving) {
		//移動する差分の計算
		XMFLOAT3 diffPos = mTargetPos - mOwner->getPosition();
		float moveLength = deltaTime * mMoveSpeed;

		//位置の更新
		if (fabsf(diffPos.x) > moveLength || fabsf(diffPos.y) > moveLength || fabsf(diffPos.z) > moveLength) {
			mOwner->setPosition(mOwner->getPosition() + Math::normalize(diffPos) * moveLength);
		}
		//移動終了時の処理
		else
		{
			mOwner->setPosition(mTargetPos);

			//ターン経過
			mMapManager->moveToPlayerTurn();
			isMoving = false;
		}
	}

	//死亡したらActor配列から除去
	if (!isAlive) {
		mOwner->setState(Actor::State::Dead);
	}

}	

void EnemyComponent::endProccess()
{
	CharacterComponent::endProccess();
	mMapManager->setObjectDataAt(mIndexPos[0], mIndexPos[1], ObjectType::EMPTY); //自分のいるindex座標を空に
	mOwner->getGame()->removeEnemy(dynamic_pointer_cast<EnemyComponent>(shared_from_this()));
}

void EnemyComponent::setMesh(const std::shared_ptr<MeshComponent>& mesh)
{
	mMesh = mesh;
}

void EnemyComponent::setEnemyTtype(int type)
{
	mEnemyType = type;
}

void EnemyComponent::startFlash()
{
	mFlashTimer = mFlashDuration;
}

XMFLOAT3 EnemyComponent::getPosition()
{
	return mOwner->getPosition();
}

void EnemyComponent::updateFlash()
{
	if (mMesh == nullptr) return;	//メッシュがセットされていない場合は処理しない

	//点滅処理
	if(mFlashTimer > 0.0f) {
		mFlashTimer -= deltaTime;
		float intensity = max(0.0f, mFlashTimer / mFlashDuration);
		mMesh->updateFlashIntensity(intensity * intensity);
	}
}

void EnemyComponent::move()
{
	//移動時は実行できない
	if (isMoving) return;
	//エネミーターン時のみ実行
	if (mMapManager->getTurnType() == TurnType::PLAYER) return;

	//乱数生成
	std::uniform_int_distribution<> dist(0, 3);
	int direction = 1 << dist(mt);
	int targetIndexPos[2] = { mIndexPos[0], mIndexPos[1]};

	switch (direction) {
	case Direction::UP:
		targetIndexPos[1] += 1;
		mOwner->setYRot(0.0f);
		break;
	case Direction::DOWN:
		targetIndexPos[1] -= 1;
		mOwner->setYRot(XM_PI);
		break;
	case Direction::RIGHT:
		targetIndexPos[0] += 1;
		mOwner->setYRot(XM_PIDIV2);
		break;
	case Direction::LEFT:
		targetIndexPos[0] -= 1;
		mOwner->setYRot(-XM_PIDIV2);
		break;
	}

	//進先に障害物がある場合移動不可
	if (mMapManager->getMapDataAt(targetIndexPos[0], targetIndexPos[1]) == TileType::WALL ||
		mMapManager->getObjectDataAt(targetIndexPos[0], targetIndexPos[1]) != ObjectType::EMPTY) return;

	mTargetPos = XMFLOAT3(targetIndexPos[0] * MAPTIPSIZE, 0.0f, targetIndexPos[1] * MAPTIPSIZE);
	mMapManager->setObjectDataAt(mIndexPos[0], mIndexPos[1], ObjectType::EMPTY); //元居た場所を空に
	mMapManager->setObjectDataAt(targetIndexPos[0], targetIndexPos[1], mEnemyType); //移動先のデータを先に更新する

	isMoving = true;
}
