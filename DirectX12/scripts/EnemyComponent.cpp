#include "EnemyComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h";

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
	isActive = false;
	mTargetPos = mOwner->getPosition();
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

	switch (mMapManager->getTurnType()) {
		//プレイヤーターン時の動作
	case TurnType::PLAYER:
		break;
		//エネミーターン時の動作
	case TurnType::ENEMY:


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
				isMoving = false;
				finishAct();
			}
		}

		//activeな時にのみ行う
		if (isActive) 
		{
			attack();
			move();
		}

		break;
	}

	//死亡したらActor配列から除去
	if (!isAlive) {
		mOwner->setState(Actor::State::Dead);
		finishAct();
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

void EnemyComponent::activate()
{
	isActive = true;
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
	//行動済みならスキップ
	if (!isActive) return;

	//乱数生成
	std::uniform_int_distribution<> dist(0, 4);
	int direction = 1 << dist(mt);
	int targetIndexPos[2] = { mIndexPos[0], mIndexPos[1]};

	switch (direction) {
	case Direction::UP:
		targetIndexPos[1] += 1;
		break;
	case Direction::DOWN:
		targetIndexPos[1] -= 1;
		break;
	case Direction::RIGHT:
		targetIndexPos[0] += 1;
		break;
	case Direction::LEFT:
		targetIndexPos[0] -= 1;
		break;
	case 1 << 4:
		isMoving = true;
		mTargetPos = mOwner->getPosition();
		return;
		break;
	}

	//進先に障害物がある場合移動不可
	if (mMapManager->getMapDataAt(targetIndexPos[0], targetIndexPos[1]) == TileType::WALL ||
		mMapManager->getObjectDataAt(targetIndexPos[0], targetIndexPos[1]) != ObjectType::EMPTY) return;

	//進行方向へ向きを変更
	switch (direction) {
	case Direction::UP:
		mOwner->setYRot(0.0f);
		break;
	case Direction::DOWN:
		mOwner->setYRot(XM_PI);
		break;
	case Direction::RIGHT:
		mOwner->setYRot(XM_PIDIV2);
		break;
	case Direction::LEFT:
		mOwner->setYRot(-XM_PIDIV2);
		break;
	}
	mTargetPos = XMFLOAT3(targetIndexPos[0] * MAPTIPSIZE, 0.0f, targetIndexPos[1] * MAPTIPSIZE);
	mMapManager->setObjectDataAt(mIndexPos[0], mIndexPos[1], ObjectType::EMPTY); //元居た場所を空に
	mMapManager->setObjectDataAt(targetIndexPos[0], targetIndexPos[1], mEnemyType); //移動先のデータを先に更新する

	isMoving = true;
}

void EnemyComponent::attack()
{
	//移動時は実行できない
	if (isMoving) return;
	//エネミーターン時のみ実行
	if (mMapManager->getTurnType() == TurnType::PLAYER) return;
	//行動済みならスキップ
	if (!isActive) return;

	//プレイヤーのインデックス座標を取得
	std::shared_ptr<Player> player = mOwner->getGame()->getPlayer();
	int playerIndexPos[2];
	player->getIndexPos(playerIndexPos);

	int diffIndexPos[2] = { playerIndexPos[0] - mIndexPos[0], playerIndexPos[1] - mIndexPos[1] };

	//プレイヤーが左側
	if (diffIndexPos[0] == -1 && diffIndexPos[1] == 0) {
		mOwner->setYRot(-XM_PIDIV2);
	}
	//プレイヤーが右側
	else if (diffIndexPos[0] == 1 && diffIndexPos[1] == 0) {
		mOwner->setYRot(XM_PIDIV2);
	}
	//プレイヤーが下
	else if (diffIndexPos[0] == 0 && diffIndexPos[1] == -1) {
		mOwner->setYRot(XM_PI);
	}
	else if (diffIndexPos[0] == 0 && diffIndexPos[1] == 1) {
		mOwner->setYRot(0.0f);
	}
	else {
		return;
	}

	int damage = max(0, mPower - player->getDefense());
	player->giveDamage(damage);

	finishAct();
}

void EnemyComponent::finishAct()
{
	mMapManager->moveToPlayerTurn();
	isActive = false;

}
