#include "TurnState.h"
#include "TurnStateMachine.h"
#include "MapGenerator.h"
#include "DungeonScene.h"
#include "EnemyComponent.h"
#include "Player.h"
#include "Random.h"
#include "Object.h"
#include "Game.h"
#include "Graphic.h"
#include "AudioManager.h"
#include "input.h"
#include "SpriteComponent.h"

//---プレイヤー入力待ち---
void PlayerTurnState::enter()
{
	//プレイヤーを入力待ち状態にする
	mPlayer.beginTurn();
}

TurnState* PlayerTurnState::update()
{
	//プレイヤーの状態に応じて遷移する
	switch (mPlayer.getStateType()) {
	case PlayerStateType::GOAL:				//ゴールに到達した
		return mOwner.getGoalState();
	case PlayerStateType::RETURN_REQUEST:	//街への帰還を要求した
		return mOwner.getReturnConfirmState();
	case PlayerStateType::ACTING:			//行動を実行した
		return mOwner.getEnemyActionState();
	case PlayerStateType::WAIT:				//行動が終了した
		return mOwner.getEnemyActionState();
	default:								//入力待ち・行動中
		return nullptr;
	}
}

//---敵行動中---
void EnemyActionState::enter()
{
	//敵配列をプレイヤーに近い順にソート
	mDungeonScene.sortEnemiesByDistanceToPlayer();
    
    //すべての敵に行動開始通知を送る
	for (auto enemy : mDungeonScene.getEnemies()) {
		enemy->startAct();
	}
}

TurnState* EnemyActionState::update()
{
	//全ての敵の行動が終了したらターン終了へ
	for (auto enemy : mDungeonScene.getEnemies()) {
		if (enemy->isActing()) return nullptr;	//まだ動いている敵がいる
	}
	
	//すべての敵が行動終了したら、ミニマップの表示を更新する。	
	mDungeonScene.updateMiniMapPos();
	return mOwner.getTurnEndState(); 
}

//---ターン終了---
TurnState* TurnEndState::update()
{
	//プレイヤーが待機状態になるまで待つ
	if (mPlayer.getStateType() != PlayerStateType::WAIT) return nullptr;

	//プレイヤーへのダメージ処理を開始
	mPlayer.damagedProcess();

	//プレイヤーが死亡していたら死亡状態へ
	if (mPlayer.getCurrentHP() <= 0) {
		return mOwner.getDeadState();
	}

	//プレイヤーの残り行動回数が0ならばAP切れ状態へ
	if (mDungeonScene.getPlayerActLimit() == 0) {
		return mOwner.getRunOutState();
	}

	//ターンを継続する場合のみ、敵のランダム湧きを行う
	spawnEnemyAndResource();

	return mOwner.getPlayerTurnState();
}

void TurnEndState::spawnEnemyAndResource()
{
	int random = Random::dist(1, 100);
	if (random <= 10)
		spawnEnemy();
	random = Random::dist(1, 100);
	if (random <= 10)
		mDungeonScene.spawnResource();
}

void TurnEndState::spawnEnemy()
{
	int playerIndex[2];
	mDungeonScene.getPlayer()->getIndexPos(playerIndex);

	int i = 0; //湧き場がない場合、一定回数のループ後にループを抜ける

	//障害物がない　かつ　プレイヤーから4マス以上9マス以下の範囲でスポーン
	int mapSize = mDungeonScene.getMapSize();
	while (i < 50) {
		//スポーンするマスを乱数で決定
		int x = playerIndex[0] + Random::dist(-9, 9);
		int y = playerIndex[1] + Random::dist(-9, 9);


		//障害物がある場合、もう一度乱数を振りなおす
		if (mDungeonScene.getTileDataAt(x, y) != TileType::FLOOR ||
			mDungeonScene.getCharacterDataAt(x, y) != CharacterType::EMPTY) {
			i++;
			continue;
		}

		//プレイヤーから3マスいないならば、もう一度乱数を振りなおす
		int distance = abs(playerIndex[0] - x) + abs(playerIndex[1] - y);
		if (distance <= 3) {
			i++;
			continue;
		}

		//敵の生成
		mDungeonScene.createEnemy("SLIME", static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
		break;
	}
}

//---街への帰還確認---
void ReturnConfirmState::enter()
{
	mSelectedIndex = YES;
	isClosing = false;
	//SEを流す
	mDungeonScene.getGame().getAudioManager().playSE("UI_WINDOW_OPEN");

	//エディターで作成した確認ウィンドウを生成
	auto window = std::make_unique<Object>(mDungeonScene, "ReturnToTownWindow");
	mWindow = window.get();
	mArrow = mWindow->getLabeledComponent<SpriteComponent>("Arrow");
	mDungeonScene.addActor(std::move(window));
}

void ReturnConfirmState::exit()
{
	//ウィンドウの所有権はシーンにあるため、参照のみ破棄する
	mWindow = nullptr;
	mArrow = nullptr;
}

TurnState* ReturnConfirmState::update()
{
	//ウィンドウを閉じた次のフレームでプレイヤー入力待ちへ戻る
	if (isClosing) {
		return mOwner.getPlayerTurnState();
	}

	//カーソル移動
	if (isKeyJustPressed(VK_RIGHT) || isKeyJustPressed('D')) {
		moveCursor(1);
	}
	else if (isKeyJustPressed(VK_LEFT) || isKeyJustPressed('A')) {
		moveCursor(-1);
	}

	//決定
	if (isKeyJustPressed(VK_RETURN) || isKeyJustPressed('K')) {
		//帰還する場合、確認ウィンドウは表示したままフェードアウトする
		if (mSelectedIndex == YES) return mOwner.getReturnState();

		isClosing = true;
	}

	//キャンセル
	if (isKeyJustPressed(VK_ESCAPE)) {
		isClosing = true;
	}

	//ウィンドウを閉じる
	if (isClosing) {
		mDungeonScene.getGame().getAudioManager().playSE("UI_WINDOW_CLOSE");
		mWindow->setState(Actor::State::Dead);
	}

	return nullptr;
}

void ReturnConfirmState::moveCursor(int diff)
{
	int next = mSelectedIndex + diff;
	if (next < 0 || next >= SELECT_NUM) {
		mDungeonScene.getGame().getAudioManager().playSE("UI_CANCEL");
		return;
	}

	mSelectedIndex = next;
	if (mArrow) mArrow->movePosition(XMFLOAT2(ArrowMoveLength * diff, 0.0f));
	mDungeonScene.getGame().getAudioManager().playSE("UI_MOVE1");
}

//---ダンジョン終了(基底)---
DungeonEndState::DungeonEndState(TurnStateMachine& owner, DungeonScene& scene, const std::string& windowID)
	: TurnState(owner),
	mDungeonScene(scene),
	mWindowID(windowID),
	mTimer(0),
	isTransitting(false),
	isTransitted(false)
{
}

void DungeonEndState::enter()
{
	mTimer = 0;
	isTransitting = false;
	isTransitted = false;

	//エディターで作成した終了ウィンドウを生成
	if (!mWindowID.empty()) {
		auto window = std::make_unique<Object>(mDungeonScene, mWindowID);
		mDungeonScene.addActor(std::move(window));
	}

	//終了時の演出
	playEndEffect();
}

TurnState* DungeonEndState::update()
{
	Graphic& graphic = mDungeonScene.getGame().getGraphic();

	//フェードアウト完了後にシーン遷移
	if (isTransitting) {
		if (!isTransitted && graphic.isFinishedFade()) {
			transitScene();
			isTransitted = true;
		}
		return nullptr;
	}

	if (waitsForInput()) {
		//ウィンドウ表示直後は入力を受け付けない
		if (mTimer < InputWaitFrame) {
			mTimer++;
			return nullptr;
		}

		//決定キーが押されるまで待機
		if (!isKeyJustPressed(VK_RETURN) && !isKeyJustPressed('K')) return nullptr;
	}

	//フェードアウト開始
	isTransitting = true;
	graphic.startFadeOut(1.0f);
	onStartTransit();

	//終了状態からは遷移しない
	return nullptr;
}

//---ゴール到達---
void GoalState::playEndEffect()
{
	//BGMを止めてゴールSEを再生
	AudioManager& audio = mDungeonScene.getGame().getAudioManager();
	audio.finishAllSounds();
	audio.playSE("GOAL");
}

void GoalState::transitScene()
{
	//現在は街へ戻るだけだが、次の階の生成・移動などはここに書くことになる
	mDungeonScene.returnToTown();
}

//---プレイヤー死亡---
void DeadState::playEndEffect()
{
	//BGMを止めて終了SEを再生
	AudioManager& audio = mDungeonScene.getGame().getAudioManager();
	audio.finishAllSounds();
	audio.playSE("DUNGEON_END");
}

void DeadState::transitScene()
{
	mDungeonScene.transitToGameOver();
}

//---AP切れ---
void RunOutState::playEndEffect()
{
	mDungeonScene.getGame().getAudioManager().playSE("DUNGEON_END");
}

void RunOutState::onStartTransit()
{
	mDungeonScene.getGame().getAudioManager().playSE("ESCAPE");
}

void RunOutState::transitScene()
{
	mDungeonScene.returnToTown();
}

//---街への帰還---
void ReturnState::onStartTransit()
{
	mDungeonScene.getGame().getAudioManager().playSE("ESCAPE");
}

void ReturnState::transitScene()
{
	mDungeonScene.returnToTown();
}
