#pragma once
#include "Actor.h"
#include "Object.h"
#include "Definition.h"
#include "AttackType.h"
#include "PlayerState.h"
#include "json_fwd.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

//前方宣言
class CameraComponent;
class CharacterComponent;
class MapGenerator;
class PlayerManager;
class ItemManager;
class DungeonScene;

//PlayerManagerがプレイヤーデータを管理し、Playerクラスはダンジョンシーン中のプレイヤーの位置やHPなどのステータスを管理する
//行動は各状態(PlayerState)から各コンポーネントに委託し、Playerクラスは状態の管理と全状態共通の処理を行う
class Player : public Actor
{
public:
	Player(DungeonScene& scene, float x, float y);
	DECLARE_CLASS_NAME(PLAYER)

	void inputActor() override;			//入力処理
	void updateActor() override;		//毎フレームの更新処理
	void endProcessActor() override;	//コンポーネントが破棄される際の処理

	//getter
	int getDirection();
	void getIndexPos(int(&pos)[2]);
	std::vector<int> getIndexPos();
	int getCurrentHP();
	int getCurrentAP();
	int getPower();
	int getDefence();
	int getSelectItemIndex();
	const std::string& getSelectItemID();
	PlayerStateType getStateType() const;	//現在の状態の種類を取得

	//setter
	void setIndexPos(int x, int y);

	//プレイヤーにダメージを与える
	void giveDamage(int damage);
	void damagedProcess();				//ダメージを受けたときの処理
	//APを回復する
	void recoverAP(int value);

	//---外部から状態遷移を要求する窓口---
	//現在の状態が遷移元として正しい場合のみ遷移し、それ以外は無視する
	void beginTurn();	//待機・帰還要求 → 入力待ち (TurnStateから呼び出す)
	void startAction();	//入力待ち → 行動中 (コンポーネントから呼び出す)
	void endAction();	//入力待ち(即時に終わる行動)・行動中 → 待機 (コンポーネントから呼び出す)、APを消費する

private:
	//状態クラスのみがchangeStateで任意の状態へ遷移できる
	friend class PlayerState;
	void changeState(PlayerStateType next);

	void updateFlash();					//点滅処理の更新

	float	 mMoveSpeed;		//移動速度
	XMFLOAT3 mTargetPos;		//移動先の座標
	int		 mPendingDamage;	//次の更新でHPから引かれるダメージ量
	float	 mFlashTimer;		//ダメージを受けたときの点滅用タイマー
	float	 mFlashDuration;	//ダメージを受けたときの点滅時間
	int		 mAP;				//現在のAP

	//状態
	std::unordered_map<PlayerStateType, std::unique_ptr<PlayerState>> mStates;
	PlayerState* mCurrentState;

	//コンポーネント
	CameraComponent* mCamera;
	CharacterComponent* mCharacter;
	class PlayerItemComponent* mItemComponent;

	//データ管理クラスやシーンへの参照
	ItemManager& mItemManager;
	PlayerManager& mPlayerManager;
	DungeonScene& mScene;
	const struct PlayerData& mPlayerData;	//プレイヤーデータの参照
};
