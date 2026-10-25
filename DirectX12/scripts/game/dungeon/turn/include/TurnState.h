#pragma once
#include <string>

enum class TurnType;
class TurnStateMachine;
class DungeonScene;
class Player;
class Object;
class SpriteComponent;

class TurnState{
public:
    virtual ~TurnState() = default;

    virtual void enter() {};
    virtual void exit() {};

    //状態の更新関数、遷移先の状態を返す。遷移しない場合はnullptr
    virtual TurnState* update() { return nullptr;}


protected:
	explicit TurnState(TurnStateMachine& owner) : mOwner(owner) {}
	TurnStateMachine& mOwner;


};

//プレイヤーのターン
//プレイヤーを入力待ち状態にし、プレイヤーの状態に応じて敵の行動・ゴール・帰還確認へ遷移
class PlayerTurnState : public TurnState {
public:
	explicit PlayerTurnState(TurnStateMachine& owner, Player& player) : TurnState(owner), mPlayer(player) {}

	void enter() override;
	TurnState* update() override;

private:
	Player& mPlayer;
};

//敵の行動中状態
//全ての敵の行動が終了したらターン終了状態へ遷移
class EnemyActionState : public TurnState {
public:
	explicit EnemyActionState(TurnStateMachine& owner, DungeonScene& scene) : TurnState(owner), mDungeonScene(scene) {}

	void enter() override;
	TurnState* update() override;
    
private:
    DungeonScene& mDungeonScene;
};

//ターン終了状態
//死亡・AP切れなら終了状態へ、継続する場合はターン終了時の処理を行いプレイヤー入力待ち状態へ遷移
class TurnEndState : public TurnState {
public:
	explicit TurnEndState(TurnStateMachine& owner, DungeonScene& scene, Player& player) : TurnState(owner), mDungeonScene(scene), mPlayer(player) {}

	TurnState* update() override;

private:
	void spawnEnemyAndResource();

	

	DungeonScene& mDungeonScene;
	Player& mPlayer;
};

//街への帰還確認状態
//確認ウィンドウを表示し、「はい」で帰還状態へ、「いいえ」かESCでプレイヤー入力待ち状態へ遷移
class ReturnConfirmState : public TurnState {
public:
	ReturnConfirmState(TurnStateMachine& owner, DungeonScene& scene)
		: TurnState(owner), mDungeonScene(scene), mWindow(nullptr), mArrow(nullptr), mSelectedIndex(YES), isClosing(false) {}

	void enter() override;
	void exit() override;
	TurnState* update() override;

private:
	enum Select {
		YES,
		NO,
		SELECT_NUM,
	};
	static constexpr float ArrowMoveLength = 160.0f;	//選択肢1つ分の矢印の移動量(px)

	void moveCursor(int diff);

	DungeonScene& mDungeonScene;
	Object* mWindow;			//確認ウィンドウ(エディターで作成)
	SpriteComponent* mArrow;	//選択中の項目を示す矢印
	int mSelectedIndex;
	bool isClosing;				//ウィンドウを閉じた直後か(同じキー入力をプレイヤーが受け取らないよう、1フレーム後に遷移する)
};

//ダンジョン終了状態の基底クラス
//終了ウィンドウを表示し、決定キーでフェードアウトした後にシーン遷移する。この状態からは他の状態へ遷移しない
class DungeonEndState : public TurnState {
public:
	void enter() override;
	TurnState* update() override;

protected:
	//windowIDが空の場合はウィンドウを生成しない
	DungeonEndState(TurnStateMachine& owner, DungeonScene& scene, const std::string& windowID);

	//決定キーの入力を待ってからフェードアウトするか(falseの場合は即座にフェードアウトを開始する)
	virtual bool waitsForInput() const { return true; }
	//終了時の演出(SE、BGMなど)
	virtual void playEndEffect() {}
	//フェードアウト開始時の演出
	virtual void onStartTransit() {}
	//フェードアウト完了後のシーン遷移
	virtual void transitScene() = 0;

	DungeonScene& mDungeonScene;

private:
	static constexpr int InputWaitFrame = 10;	//誤入力防止のため、入力を受け付けるまでのフレーム数

	const std::string mWindowID;	//エディターで作成した終了ウィンドウのオブジェクトID
	int mTimer;
	bool isTransitting;
	bool isTransitted;
};

//ゴール到達
class GoalState : public DungeonEndState {
public:
	GoalState(TurnStateMachine& owner, DungeonScene& scene) : DungeonEndState(owner, scene, "GoalWindow") {}

private:
	void playEndEffect() override;
	void transitScene() override;
};

//プレイヤー死亡
class DeadState : public DungeonEndState {
public:
	DeadState(TurnStateMachine& owner, DungeonScene& scene) : DungeonEndState(owner, scene, "DeadWindow") {}

private:
	void playEndEffect() override;
	void transitScene() override;
};

//AP切れ
class RunOutState : public DungeonEndState {
public:
	RunOutState(TurnStateMachine& owner, DungeonScene& scene) : DungeonEndState(owner, scene, "RunOutWindow") {}

private:
	void playEndEffect() override;
	void onStartTransit() override;
	void transitScene() override;
};

//街への帰還(確認ウィンドウで「はい」を選択)
//確認ウィンドウを表示したまま、入力を待たずにフェードアウトする
class ReturnState : public DungeonEndState {
public:
	ReturnState(TurnStateMachine& owner, DungeonScene& scene) : DungeonEndState(owner, scene, "") {}

private:
	bool waitsForInput() const override { return false; }
	void onStartTransit() override;
	void transitScene() override;
};