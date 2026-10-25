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
	mScene(scene),
	mPlayerData(scene.getGame().getPlayerManager().getPlayerData())
{
	//初期化
	mMoveStrategy	= std::make_unique<RandomWalkStrategy>();
	isActive		= false;
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
	//すでに死亡していたら行動しない
	if(!isAlive) return;
	//ターン経過するアクションを実行可能にする
	isActive = true;

	//ターン経過するアクション
	attack();	//プレイヤーが隣接していたら攻撃
	move();		//行動パターンに基づいて移動先を決定
	
	//プレイヤーとの距離を計算 プレイヤーに近いエネミーから行動するようにエネミー配列をソートするため。
	int playerIndex[2];
	mScene.getPlayer()->getIndexPos(playerIndex);
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
	const std::array<int, 2> targetIndexPos = mMoveStrategy->calcTargetIndex(*this, mScene);

	//進先に障害物がある場合移動不可
	if (mScene.getTileDataAt(targetIndexPos[0], targetIndexPos[1]) == TileType::WALL ||
		mScene.getCharacterDataAt(targetIndexPos[0], targetIndexPos[1]) != CharacterType::EMPTY) {
		mTargetPos = mOwner.getPosition();
		return;
	}

	//向きの変更
	//右向き
	if (targetIndexPos[0] - mIndexPos[0] == 1) mOwner.setYRot(XM_PIDIV2);
	//左向き
	else if (targetIndexPos[0] - mIndexPos[0] == -1) mOwner.setYRot(-XM_PIDIV2);
	//上向き
	else if (targetIndexPos[1] - mIndexPos[1] == 1) mOwner.setYRot(0.0f);
	//下向き
	else if (targetIndexPos[1] - mIndexPos[1] == -1) mOwner.setYRot(XM_PI);

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

	//プレイヤーのインデックス座標を取得
	Player* player = mScene.getPlayer();
	int playerIndexPos[2];
	player->getIndexPos(playerIndexPos);

	//プレイヤーとの位置の差を計算
	int diffIndexPos[2] = { playerIndexPos[0] - mIndexPos[0], playerIndexPos[1] - mIndexPos[1] };

	//プレイヤーの位置によって向きを変える
	//プレイヤーが左側
	if (diffIndexPos[0] == -1 && diffIndexPos[1] == 0) {
		mOwner.setYRot(-XM_PIDIV2);
	}
	//プレイヤーが右側
	else if (diffIndexPos[0] == 1 && diffIndexPos[1] == 0) {
		mOwner.setYRot(XM_PIDIV2);
	}
	//プレイヤーが下
	else if (diffIndexPos[0] == 0 && diffIndexPos[1] == -1) {
		mOwner.setYRot(XM_PI);
	}
	else if (diffIndexPos[0] == 0 && diffIndexPos[1] == 1) {
		mOwner.setYRot(0.0f);
	}
	else {
		return;
	}

	//ダメージを計算し、プレイヤーのHPを減らす
	int damage = max(0, mPower - mPlayerData.defence);
	player->giveDamage(damage);

	//行動終了
	finishAct();
}

void EnemyComponent::finishAct()
{
	if(!isActive) return;
	//行動フラグをfalseに（ターン制御クラスがフラグを見て敵ターンの終了を検知）
	isActive = false;
}

void EnemyComponent::deadProcess()
{
	//ステートの変更
	mOwner.setState(Actor::State::Dead);
	//お金の取得
	mScene.getGame().getItemManager().addResource("GOLD", mDropMoney);
		
	//メッセージの追加
	std::string message = mName + "を倒した! " + std::to_string(mDropMoney) + "G取得\n";
	mScene.pushMessage(message);
	mScene.updateGold();

	finishAct();
}