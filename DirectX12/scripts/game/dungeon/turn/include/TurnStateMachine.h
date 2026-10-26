#pragma once
#include "Actor.h"
#include "TurnState.h"
#include <memory>

enum class TurnType;
class DungeonScene;

class TurnStateMachine
{
public:
	TurnStateMachine(DungeonScene& scene);

	void updateTurn();
	void begin();

	//各状態の取得(状態遷移先の指定に使用)
	TurnState* getPlayerTurnState() const { return mPlayerTurnState.get(); }
	TurnState* getEnemyActionState() const { return mEnemyActionState.get(); }
	TurnState* getTurnEndState() const { return mTurnEndState.get(); }
	TurnState* getGoalState() const { return mGoalState.get(); }
	TurnState* getDeadState() const { return mDeadState.get(); }
	TurnState* getRunOutState() const { return mRunOutState.get(); }
	TurnState* getReturnConfirmState() const { return mReturnConfirmState.get(); }
	TurnState* getReturnState() const { return mReturnState.get(); }

	//ターン制御
	void stop();
	void start();

private:
	void changeState(TurnState* next);

	DungeonScene& mScene;
	bool mIsActive;

	//状態
	std::unique_ptr<TurnState> mPlayerTurnState;
	std::unique_ptr<TurnState> mEnemyActionState;
	std::unique_ptr<TurnState> mTurnEndState;
	std::unique_ptr<TurnState> mReturnConfirmState;
	//ダンジョン終了状態
	std::unique_ptr<TurnState> mGoalState;
	std::unique_ptr<TurnState> mDeadState;
	std::unique_ptr<TurnState> mRunOutState;
	std::unique_ptr<TurnState> mReturnState;
	TurnState* mCurrentState;

};