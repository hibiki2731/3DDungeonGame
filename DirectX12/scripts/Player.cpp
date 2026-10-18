#include "Player.h"
#include "CameraComponent.h"
#include "SpotLightComponent.h"
#include "timer.h"
#include "CharacterComponent.h"
#include <windows.h>
#include <algorithm>
#include "Game.h"
#include "Graphic.h"
#include "ItemManager.h"
#include "PlayerManager.h"
#include "AudioManager.h"
#include "DungeonScene.h"
#include "PlayerAttackComponent.h"
#include "PlayerMoveComponent.h"
#include "PlayerRotateComponent.h"
#include "PlayerItemComponent.h"
#include "PlayerInteractComponent.h"
#include "MapGenerator.h"

Player::Player(DungeonScene& scene, float x, float y)
	: Actor(scene),
	mScene(scene),
	mPlayerManager(scene.getGame().getPlayerManager()),
	mItemManager(scene.getGame().getItemManager()),
	mPlayerData(mPlayerManager.getPlayerData())
{
	//初期化
	mPosition		 = { x, 0.8f, y };
	mTargetPos	     = mPosition;
	mPendingDamage	 = 0;
	mFlashTimer		 = 0.0f;

	//カメラコンポーネントの生成：プレイヤーの視点を制御
	std::unique_ptr camera = std::make_unique<CameraComponent>(*this);
	camera->setActive(true);
	mCamera = camera.get();
	addComponent(std::move(camera));

	//スポットライトコンポーネントの生成：プレイヤーの視点方向を照らすライト
	std::unique_ptr<SpotLightComponent> spotLight = std::make_unique<SpotLightComponent>(*this);
	spotLight->setActive(true);
	spotLight->setColor(XMFLOAT4(1.0f, 0.9f, 0.8f, 1.0f));
	spotLight->setIntensity(20.0f);
	spotLight->setRange(50.0f);
	spotLight->setUAngle(XMConvertToRadians(10.0f));
	spotLight->setPAngle(XMConvertToRadians(40.0f));
	spotLight->setOffsetPos(XMFLOAT4(0.0f, 0.2f, 0.0f, 0.0f));
	addComponent(std::move(spotLight));

	//プレイヤーデータから各種パラメータを取得
	//探索道具の効果をステータスに適用する
	mPlayerManager.applyToolParamater();

	mMoveSpeed		= mPlayerData.moveSpeed;		//移動速度
	mFlashDuration  = mPlayerData.flushDuration;	//ダメージを受けたときの点滅時間
	mAP = mPlayerData.actionLimit;					//APの初期値

	//キャラクターコンポーネントの生成
	auto character = std::make_unique<CharacterComponent>(*this, scene);
	character->setDirection(Direction::UP);
	character->setIndexPos(static_cast<int>(std::round(x / MAPTIPSIZE)), static_cast<int>(std::round(y / MAPTIPSIZE)));
	character->setMaxHP(mPlayerData.maxHp);
	character->setHP(mPlayerData.hp);
	character->setPower(mPlayerData.power);
	character->setDefense(mPlayerData.defence);
	mCharacter = character.get();
	addComponent(std::move(character));

	//---行動用コンポーネントの生成---
	//移動
	auto moveComponent = std::make_unique<PlayerMoveComponent>(scene, *this);
	PlayerMoveComponent& move = *moveComponent;
	addComponent(std::move(moveComponent));

	//攻撃 攻撃タイプに対応する攻撃コンポーネントを入力待ち状態に渡す
	auto singleAttack = std::make_unique<PlayerSingleAttackComponent>(scene, *this);
	auto doubleAttack = std::make_unique<PlayerDoubleAttackComponent>(scene, *this);
	PlayerAttackComponent* attack = nullptr;
	switch (mPlayerData.attackType) {
	case AttackType::SINGLE:
		attack = singleAttack.get();
		break;
	case AttackType::DOUBLE:
		attack = doubleAttack.get();
		break;
	}
	addComponent(std::move(singleAttack));
	addComponent(std::move(doubleAttack));

	//視点移動
	auto rotateComponent = std::make_unique<PlayerRotateComponent>(scene, *this, *mCharacter, mPlayerData.rotSpeed);
	PlayerRotateComponent& rotate = *rotateComponent;
	addComponent(std::move(rotateComponent));

	//アイテム
	auto itemComponent = std::make_unique<PlayerItemComponent>(scene, *this, *mCharacter);
	mItemComponent = itemComponent.get();
	addComponent(std::move(itemComponent));

	//フィールド上の物への行動(資源、宝箱、階段)
	auto interactComponent = std::make_unique<PlayerInteractComponent>(scene, *this, *mCharacter);
	PlayerInteractComponent& interact = *interactComponent;
	addComponent(std::move(interactComponent));

	//---状態の生成---
	mStates[PlayerStateType::REQUEST_ACTION] = std::make_unique<PlayerRequestActionState>(*this, move, attack, rotate, *mItemComponent, interact);
	mStates[PlayerStateType::ACTING]		 = std::make_unique<PlayerActingState>(*this);
	mStates[PlayerStateType::WAIT]			 = std::make_unique<PlayerWaitState>(*this);
	mStates[PlayerStateType::GOAL]			 = std::make_unique<PlayerGoalState>(*this);
	mStates[PlayerStateType::RETURN_REQUEST] = std::make_unique<PlayerReturnRequestState>(*this);

	//TurnStateMachineからbeginTurnが呼ばれるまで待機する
	mCurrentState = mStates[PlayerStateType::WAIT].get();
	mCurrentState->enter();
}

void Player::inputActor()
{
	mCurrentState->input();
}

//毎フレームの更新処理
void Player::updateActor()
{
	mCurrentState->update();

	//ダメージを受けたときの点滅処理(全状態共通)
	updateFlash();
}

//アクター破棄時の処理
void Player::endProcessActor()
{
	//ダンジョンシーン中のデータをプレイヤーマネージャーに保存する
	PlayerManager& player = mScene.getGame().getPlayerManager();
	player.setHP(mCharacter->getHP());
	//点滅強度をリセットする
	mScene.getGame().getGraphic().updateDamageFlashIntensity(0.0f);
}

//---getter---
int Player::getDirection()
{
	return mCharacter->getDirection();
}

void Player::getIndexPos(int(&pos)[2])
{
	pos[0] = mCharacter->getIndexPos()[0];
	pos[1] = mCharacter->getIndexPos()[1];
}

std::vector<int> Player::getIndexPos()
{
	std::vector<int> pos = { mCharacter->getIndexPos()[0], mCharacter->getIndexPos()[1] };
	return pos;
}

int Player::getCurrentHP()
{
	return mCharacter->getHP();
}

int Player::getCurrentAP()
{
	return mAP;
}

int Player::getPower()
{
	return mCharacter->getPower();
}

int Player::getDefence()
{
	return mCharacter->getDefence();
}

int Player::getSelectItemIndex()
{
	return mItemComponent->getSelectItemIndex();
}

const std::string& Player::getSelectItemID()
{
	return mItemComponent->getSelectItemID();
}

PlayerStateType Player::getStateType() const
{
	return mCurrentState->getType();
}

void Player::setIndexPos(int x, int y)
{
	mCharacter->setIndexPos(x, y);
}

//プレイヤーにダメージを与える
void Player::giveDamage(int damage)
{
	//直接HPを減らさない
	//行動中に受けたダメージを全て加算し、行動終了後にまとめてHPから引くようにする
	mPendingDamage += damage;
}

void Player::damagedProcess()
{
	//ダメージの反映
	if (mPendingDamage <= 0) return;
	int hp = mCharacter->getHP() - mPendingDamage;
	mCharacter->setHP(hp);
	mPendingDamage = 0;

	//UIの更新
	mScene.updateHPUI();

	//被ダメージ時の演出
	mFlashTimer = mFlashDuration;	//画面を点滅
	mCamera->startShake();			//カメラを揺らす
	mScene.getGame().getAudioManager().playSE("DAMAGE2");
}

void Player::recoverAP(int value)
{
	mAP += value;
	//アイテム使用でAPが1消費されるため、上限+1までにする
	if (mAP > mPlayerData.actionLimit) mAP = mPlayerData.actionLimit + 1;
}

void Player::updateFlash()
{
	//ダメージの点滅処理
	if (mFlashTimer > 0.0f) {
		mFlashTimer -= deltaTime;
		float intensity = max(0.0f, mFlashTimer / mFlashDuration);
		mScene.getGame().getGraphic().updateDamageFlashIntensity(intensity);	//コンスタントバッファに点滅の強度を送る
	}
}

//---状態遷移---
void Player::beginTurn()
{
	const PlayerStateType type = getStateType();
	if (type != PlayerStateType::WAIT && type != PlayerStateType::RETURN_REQUEST) return;

	changeState(PlayerStateType::REQUEST_ACTION);
}

void Player::startAction()
{
	if (getStateType() != PlayerStateType::REQUEST_ACTION) return;

	changeState(PlayerStateType::ACTING);
}

void Player::endAction()
{
	const PlayerStateType type = getStateType();
	if (type != PlayerStateType::REQUEST_ACTION && type != PlayerStateType::ACTING) return;

	//残り行動回数を減らす
	mAP--;
	//UIの更新
	if(mAP >= 0) mScene.updateAPUI();

	changeState(PlayerStateType::WAIT);
}

void Player::changeState(PlayerStateType next)
{
	mCurrentState->exit();
	mCurrentState = mStates.at(next).get();
	mCurrentState->enter();
}
