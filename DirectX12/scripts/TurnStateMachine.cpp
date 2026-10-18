#include "TurnStateMachine.h"
#include "DungeonScene.h"
#include "Random.h"
#include "Definition.h"
#include "EnemyComponent.h"
#include "Player.h"
#include "SpriteComponent.h"
#include "TextComponent.h"
#include "input.h"
#include "Game.h"
#include "Graphic.h"
#include "AudioManager.h"

TurnStateMachine::TurnStateMachine(DungeonScene& scene)
	: mScene(scene)
{
	mPlayerActionState = nullptr;
	mEnemyActionState = nullptr;
	mTurnEndState = nullptr;
	mGoalState = nullptr;
	mDeadState = nullptr;
	mRunOutState = nullptr;
	mReturnConfirmState = nullptr;
	mReturnState = nullptr;
	mCurrentState = mPlayerActionState.get();

	mIsActive = true;
}

void TurnStateMachine::updateTurn()
{
	//停止中は状態を進めない
	if (!mIsActive) return;

	//現在の状態を更新し、遷移先が返されたら遷移する
	TurnState* next = mCurrentState->update();
	if (next) changeState(next);
}

void TurnStateMachine::begin()
{
	mIsActive = true;

	//既存の状態を破棄
	if(mPlayerActionState || mEnemyActionState || mTurnEndState)
	{
		mPlayerActionState.reset();
		mEnemyActionState.reset();
		mTurnEndState.reset();
		mGoalState.reset();
		mDeadState.reset();
		mRunOutState.reset();
		mReturnConfirmState.reset();
		mReturnState.reset();
	}

	//各状態の生成
	mPlayerActionState = std::make_unique<PlayerActionState>(*this, *mScene.getPlayer());
	mEnemyActionState = std::make_unique<EnemyActionState>(*this, mScene);
	mTurnEndState = std::make_unique<TurnEndState>(*this, mScene, *mScene.getPlayer());
	mGoalState = std::make_unique<GoalState>(*this, mScene);
	mDeadState = std::make_unique<DeadState>(*this, mScene);
	mRunOutState = std::make_unique<RunOutState>(*this, mScene);
	mReturnConfirmState = std::make_unique<ReturnConfirmState>(*this, mScene);
	mReturnState = std::make_unique<ReturnState>(*this, mScene);

	mCurrentState = mPlayerActionState.get();
	mCurrentState->enter();
}

void TurnStateMachine::stop()
{
	mIsActive = false;
}

void TurnStateMachine::start()
{
	mIsActive = true;
}

void TurnStateMachine::changeState(TurnState* next)
{
	mCurrentState->exit();
	mCurrentState = next;
	mCurrentState->enter();
}
