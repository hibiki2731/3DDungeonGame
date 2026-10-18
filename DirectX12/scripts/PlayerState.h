#pragma once
#include "Definition.h"

class Player;
class PlayerMoveComponent;
class PlayerAttackComponent;
class PlayerRotateComponent;
class PlayerItemComponent;
class PlayerInteractComponent;

//プレイヤーの状態の種類(TurnStateはこの値でプレイヤーの状態を確認する)
enum class PlayerStateType {
	REQUEST_ACTION,	//入力待ち
	ACTING,			//ターン経過する行動を実行中
	WAIT,			//何もしない(敵ターン中など)
	GOAL,			//ゴールに到達した
	RETURN_REQUEST,	//街への帰還を要求している
};

//プレイヤーの状態の基底クラス
class PlayerState {
public:
	virtual ~PlayerState() = default;

	virtual void enter() {};
	virtual void exit() {};
	virtual void update() {};
	virtual void input() {};

	virtual PlayerStateType getType() const = 0;

protected:
	explicit PlayerState(Player& owner) : mOwner(owner) {}

	//Player::changeStateへの窓口(状態クラスのみが任意の状態へ遷移できる)
	void changeState(PlayerStateType next);

	Player& mOwner;
};

//入力待ち状態
//入力に応じて各コンポーネントの行動を実行する
class PlayerRequestActionState : public PlayerState {
public:
	PlayerRequestActionState(Player& owner, PlayerMoveComponent& move, PlayerAttackComponent* attack,
		PlayerRotateComponent& rotate, PlayerItemComponent& item, PlayerInteractComponent& interact);

	void enter() override;
	void update() override;
	void input() override;
	PlayerStateType getType() const override { return PlayerStateType::REQUEST_ACTION; }

private:
	//まだ入力待ち状態か(行動を開始すると状態が変わるため、同じフレームで他の行動を受け付けないよう確認する)
	bool isAcceptingInput() const;
	void startRotate(Direction direction);
	void doAction();

	PlayerMoveComponent& mMove;
	PlayerAttackComponent* mAttack;	//攻撃タイプに対応する攻撃コンポーネント(存在しない場合はnullptr)
	PlayerRotateComponent& mRotate;
	PlayerItemComponent& mItem;
	PlayerInteractComponent& mInteract;

	bool isRotating;	//視点移動中か(回転中は入力を受け付けない)
};

//行動中状態
//行動の進行は各コンポーネントが行い、終了時にPlayer::endActionで待機状態へ遷移する
class PlayerActingState : public PlayerState {
public:
	explicit PlayerActingState(Player& owner) : PlayerState(owner) {}
	PlayerStateType getType() const override { return PlayerStateType::ACTING; }
};

//待機状態
//TurnStateからPlayer::beginTurnが呼ばれると入力待ち状態へ遷移する
class PlayerWaitState : public PlayerState {
public:
	explicit PlayerWaitState(Player& owner) : PlayerState(owner) {}
	PlayerStateType getType() const override { return PlayerStateType::WAIT; }
};

//ゴール到達状態
class PlayerGoalState : public PlayerState {
public:
	explicit PlayerGoalState(Player& owner) : PlayerState(owner) {}
	PlayerStateType getType() const override { return PlayerStateType::GOAL; }
};

//帰還要求状態
//帰還確認でキャンセルされた場合、Player::beginTurnで入力待ち状態へ戻る
class PlayerReturnRequestState : public PlayerState {
public:
	explicit PlayerReturnRequestState(Player& owner) : PlayerState(owner) {}
	PlayerStateType getType() const override { return PlayerStateType::RETURN_REQUEST; }
};
