#include "PlayerState.h"
#include "Player.h"
#include "PlayerMoveComponent.h"
#include "PlayerAttackComponent.h"
#include "PlayerRotateComponent.h"
#include "PlayerItemComponent.h"
#include "PlayerInteractComponent.h"
#include "input.h"

void PlayerState::changeState(PlayerStateType next)
{
	mOwner.changeState(next);
}

//---入力待ち---
PlayerRequestActionState::PlayerRequestActionState(Player& owner, PlayerMoveComponent& move, PlayerAttackComponent* attack,
	PlayerRotateComponent& rotate, PlayerItemComponent& item, PlayerInteractComponent& interact)
	: PlayerState(owner),
	mMove(move),
	mAttack(attack),
	mRotate(rotate),
	mItem(item),
	mInteract(interact),
	isRotating(false)
{
}

void PlayerRequestActionState::enter()
{
	isRotating = false;
}

void PlayerRequestActionState::update()
{
	//視点移動の更新、回転が完了したら入力を再開する
	if (isRotating && mRotate.updateRotate()) {
		isRotating = false;
	}
}

void PlayerRequestActionState::input()
{
	//回転中は入力を受け付けない
	if (isRotating) return;

	//---移動---
	static constexpr std::pair<int, Direction> MoveKeys[] = {
		{ 'A', Direction::LEFT },
		{ 'D', Direction::RIGHT },
		{ 'W', Direction::UP },
		{ 'S', Direction::DOWN },
	};
	for (const auto& [key, direction] : MoveKeys) {
		if (!isKeyPressed(key)) continue;
		mMove.move(direction);
		if (!isAcceptingInput()) return;	//移動を開始した
	}

	//---視点移動---
	if (isKeyPressed(VK_RIGHT) || isKeyPressed('L')) {
		startRotate(Direction::RIGHT);
		return;
	}
	if (isKeyPressed(VK_LEFT) || isKeyPressed('J')) {
		startRotate(Direction::LEFT);
		return;
	}

	//---アクション---
	if (isKeyJustPressed(VK_RETURN) || isKeyJustPressed('K')) {
		doAction();
		if (!isAcceptingInput()) return;
	}

	//---アイテム---
	if (isKeyJustPressed('I')) {
		mItem.useItem();
		if (!isAcceptingInput()) return;
	}
	if (isKeyJustPressed('U')) {
		mItem.selectPreviousItem();
	}
	if (isKeyJustPressed('O')) {
		mItem.selectNextItem();
	}

	//---街へ帰還---
	if (isKeyJustPressed(VK_ESCAPE)) {
		changeState(PlayerStateType::RETURN_REQUEST);
	}
}

bool PlayerRequestActionState::isAcceptingInput() const
{
	return mOwner.getStateType() == getType();
}

void PlayerRequestActionState::startRotate(Direction direction)
{
	mRotate.startRotate(direction);
	isRotating = true;
}

void PlayerRequestActionState::doAction()
{
	if (mAttack && mAttack->execute()) return;	//目の前の敵を攻撃
	if (mInteract.collect()) return;			//リソースを回収
	if (mInteract.openTreasure()) return;		//宝箱からアイテムを入手

	//目の前に階段があればゴール状態へ
	if (mInteract.isFacingGoal()) {
		changeState(PlayerStateType::GOAL);
	}
}
