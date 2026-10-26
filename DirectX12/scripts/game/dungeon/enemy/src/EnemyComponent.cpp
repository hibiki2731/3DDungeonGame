#include "EnemyComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h"
#include "Random.h"
#include "MyMath.h"
#include "DungeonScene.h"
#include "TurnStateMachine.h"
#include "ItemManager.h"
#include "Enemy.h"
#include "PlayerManager.h"
#include "MyMath.h"
#include "MapGenerator.h"

EnemyComponent::EnemyComponent(Actor& owner, DungeonScene& scene) 
	: CharacterComponent(owner, scene),
	mScene(scene)
{
	//初期化
	mMoveStrategy	= std::make_unique<RandomWalkStrategy>(*this, scene);
	mAttackStrategy	= std::make_unique<MeleeAttackStrategy>(*this, scene);
	isActive		= false;
	mIsDeadProcessed = false;
	mTargetPos		= mOwner.getPosition();
	mDistPlayer		= 10000000;
	mDropMoney		= 0;
	//メッシュ
	mMesh			= nullptr;
	//点滅処理
	mFlashTimer		= 0.0f;
	mFlashDuration	= 0.3f;	//ダメージを受けたときの点滅時間
	//移動
	isMoving		= false;
	mMoveSpeed		= 5.0f;
	//アクターの初期位置から、インデックス位置を計算
	mIndexPos[1] = static_cast<int>(std::round(mOwner.getPosition().z / MAPTIPSIZE));
	mIndexPos[0] = static_cast<int>(std::round(mOwner.getPosition().x / MAPTIPSIZE));
	//自分のいるマスをマップデータに登録する(途中で生成されたエネミーも障害物として扱われるようにするため)
	scene.setCharacterDataAt(mIndexPos[0], mIndexPos[1], CharacterType::ENEMY);
	//シーンのエネミーの処理用配列に追加する
	scene.addEnemy(this);
}

EnemyComponent::~EnemyComponent() = default;

void EnemyComponent::updateComponent()
{
	//点滅処理の更新
	updateFlash();

	// 移動処理
	// 移動先(mTargetPos)はmove関数内で決定
	if (isMoving)
	{
		// 移動する差分の計算
		XMFLOAT3 diffPos = mTargetPos - mOwner.getPosition();
		float moveLength = deltaTime * mMoveSpeed;

		// 位置の更新  diffPos(現在地と目的地の差分)よりmoveLength(移動距離)が大きくなるまで現在地にmoveLengthを加算
		if (fabsf(diffPos.x) > moveLength || fabsf(diffPos.y) > moveLength || fabsf(diffPos.z) > moveLength)
		{
			mOwner.setPosition(mOwner.getPosition() + Math::normalize(diffPos) * moveLength);
		}
		else
		{
			// 目的地にピッタリ合わせる
			mOwner.setPosition(mTargetPos);

			// ターン経過処理
			isMoving = false;
			finishAct();
		}
	}
	
	// 死亡したらActor配列から除去
	if (!isAlive)
	{
		deadProcess();
	}
}	

//コンポーネント除去時の処理
void EnemyComponent::endProcess()
{
	mScene.removeEnemy(this);			//シーンのエネミー配列から除去
	mScene.setCharacterDataAt(mIndexPos[0], mIndexPos[1], CharacterType::EMPTY); //自分のいるindex座標を空に
}

//エネミーターン開始時の処理
void EnemyComponent::startAct()
{
	//すでに死亡・除去されていたら行動しない
	if(!isAlive || mIsDeadProcessed) return;
	//ターン経過するアクションを実行可能にする
	isActive = true;

	//ターン経過するアクション
	attack();	//プレイヤーが隣接していたら攻撃
	move();		//行動パターンに基づいて移動先を決定
	
	//プレイヤーとの距離を計算 プレイヤーに近いエネミーから行動するようにエネミー配列をソートするため。
	const std::array<int, 2> playerIndex = mScene.getPlayer()->getIndexPos();
	mDistPlayer = abs(playerIndex[0] - mIndexPos[0]) + abs(playerIndex[1] - mIndexPos[1]);
}

//---setter---
void EnemyComponent::setMesh(MeshComponent* mesh)
{
	mMesh = mesh;
}
void EnemyComponent::setMoveStrategy(std::unique_ptr<EnemyMoveStrategy> strategy)
{
	mMoveStrategy = std::move(strategy);
}

void EnemyComponent::setAttackStrategy(std::unique_ptr<EnemyAttackStrategy> strategy)
{
	mAttackStrategy = std::move(strategy);
}

void EnemyComponent::setDropMoney(int money)
{
	mDropMoney = money;
}

void EnemyComponent::setName(const std::string& name)
{
	mName = name;
}


//---getter---
XMFLOAT3 EnemyComponent::getPosition()
{
	return mOwner.getPosition();
}
int EnemyComponent::getDist()
{
	return mDistPlayer;
}

void EnemyComponent::startFlash()
{
	mFlashTimer = mFlashDuration;
}

void EnemyComponent::updateFlash()
{
	if (mMesh == nullptr) return;	//メッシュがセットされていない場合は処理しない

	//点滅処理
	if(mFlashTimer > 0.0f) {
		mFlashTimer -= deltaTime;
		float intensity = max(0.0f, mFlashTimer / mFlashDuration);
		mMesh->updateFlashIntensity(intensity * intensity);			//メッシュのコンスタントバッファの値を更新
	}
}

void EnemyComponent::move()
{	
	//行動済みならスキップ
	if (!isActive) return;

	isMoving = true;

	//行動パターンに応じて移動先のインデックス座標をもとめる
	const std::array<int, 2> targetIndexPos = mMoveStrategy->calcTargetIndex();

	//進先に障害物がある場合移動不可
	if (!mScene.isWalkableTile(targetIndexPos[0], targetIndexPos[1]) ||
		mScene.getCharacterDataAt(targetIndexPos[0], targetIndexPos[1]) != CharacterType::EMPTY) {
		mTargetPos = mOwner.getPosition();
		return;
	}

	//移動方向を向く
	lookAt(targetIndexPos);

	//移動先のワールド座標を計算
	mTargetPos = XMFLOAT3(targetIndexPos[0] * MAPTIPSIZE, 0.0f, targetIndexPos[1] * MAPTIPSIZE);

	//マップデータや自身のインデックス座標を更新
	mScene.setCharacterDataAt(mIndexPos[0], mIndexPos[1], CharacterType::EMPTY); 			//元居た場所を空に
	mScene.setCharacterDataAt(targetIndexPos[0], targetIndexPos[1], CharacterType::ENEMY); 	//移動先のデータを先に更新する
	mIndexPos = targetIndexPos; 																//インデックス座標の更新

	//行動終了処理は移動先に到着したとき行う
}

void EnemyComponent::attack()
{
	//行動済みならスキップ
	if (!isActive) return;

	//攻撃パターンに応じて攻撃し、攻撃した場合は行動終了
	if (mAttackStrategy->tryAttack()) {
		finishAct();
	}
}

void EnemyComponent::lookAt(const std::array<int, 2>& targetIndex)
{
	//目標との位置の差を計算
	int diffX = targetIndex[0] - mIndexPos[0];
	int diffY = targetIndex[1] - mIndexPos[1];

	//より大きい距離を用いて、位置の差を正規化
	const int maxDiff = max(abs(diffX), abs(diffY));
	if (maxDiff <= 0) return;

	diffX /= maxDiff;
	diffY /= maxDiff;

	//右向き
	if (diffX == 1 && diffY == 0) mOwner.setYRot(XM_PIDIV2);
	//左向き
	else if (diffX == -1 && diffY == 0) mOwner.setYRot(-XM_PIDIV2);
	//上向き
	else if (diffX == 0 && diffY == 1) mOwner.setYRot(0.0f);
	//下向き
	else if (diffX == 0 && diffY == -1) mOwner.setYRot(XM_PI);
}

void EnemyComponent::finishAct()
{
	if(!isActive) return;
	//行動フラグをfalseに（ターン制御クラスがフラグを見て敵ターンの終了を検知）
	isActive = false;
}

void EnemyComponent::deadProcess()
{
	//死亡演出でアクターが残る場合に毎フレーム呼ばれるため、一度だけ実行する
	if (mIsDeadProcessed) return;
	mIsDeadProcessed = true;

	//お金の取得
	mScene.getGame().getItemManager().addResource("GOLD", mDropMoney);
		
	//メッセージの追加
	std::string message = mName + "を倒した! " + std::to_string(mDropMoney) + "G取得\n";
	mScene.pushMessage(message);
	mScene.updateGold();

	finishAct();

	//シーンに撃破を通知。シーンが除去を引き受けない場合はここで除去する
	if (!mScene.onEnemyDefeated(*this)) {
		mOwner.setState(Actor::State::Dead);
	}
}

void EnemyComponent::despawn()
{
	if (mIsDeadProcessed) return;
	mIsDeadProcessed = true;

	finishAct();
	mOwner.setState(Actor::State::Dead);
}