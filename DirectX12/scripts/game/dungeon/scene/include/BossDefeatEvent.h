#pragma once
#include <array>

class Dungeon2Scene;
class EnemyComponent;
class Actor;
struct IXAudio2SourceVoice;

//ボス討伐時のイベント
//敵の消去・AP消費の停止・SE/BGMの切り替え・ボスを沈める演出・ゴールへの道の生成を行う
class BossDefeatEvent
{
public:
	explicit BossDefeatEvent(Dungeon2Scene& scene);

	void start(EnemyComponent& boss);	//ボス撃破時に呼ぶ
	void update();						//毎フレーム呼ぶ
	void reset();						//シーン進入・退出時に初期化

	bool isStarted() const { return mPhase != Phase::Idle; }
	bool isPlaying() const { return mPhase == Phase::Defeated; }	//沈む演出中か

private:
	enum class Phase {
		Idle,		//未発生
		Defeated,	//撃破SE再生中(画面揺れ・ボスが沈む演出)
		Finished,	//完了
	};

	void despawnEnemies(const EnemyComponent& boss);	//ボス以外の敵を消す
	void sinkBoss();									//ボスを沈める
	void finish();										//SE終了時の処理(BGM・道・BossObject)
	void openGoalPath();								//ゴールへの道を作る

	Dungeon2Scene&			mScene;
	Phase					mPhase;
	Actor*					mBoss;			//沈める対象(除去後はnullptr)
	IXAudio2SourceVoice*	mDefeatSE;		//再生中の撃破SE(終了検知用)
	float					mBossStartY;	//沈み始める前のボスの高さ
	float					mSinkSpeed;		//沈む速度
	float					mSinkDepth;		//沈む深さ(これ以上は沈めない)
	std::array<std::array<int, 2>, 2> mGoalPathTiles;	//ゴールへの道にするマス
};
