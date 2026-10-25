#pragma once
#include <memory>
#include <array>
#include "json.hpp"

//前方宣言
class EnemyComponent;
class DungeonScene;

//---エネミーの行動パターン(ストラテジー)---
//RandomWalkStrategy	: ランダムに移動
//AstarStrategy			: A*アルゴリズムでプレイヤーを追跡
//SenseStrategy			: プレイヤーとの距離が一定値より小さくなったらA*で追跡
//BossPillarStrategy	: ボス(柱)の行動

//行動パターンの基底クラス
class EnemyMoveStrategy
{
public:
	EnemyMoveStrategy(EnemyComponent& enemy, DungeonScene& scene) : mEnemy(enemy), mScene(scene) {}
	virtual ~EnemyMoveStrategy() = default;
	//移動先のインデックス座標を計算して返す。移動しない場合は現在のインデックス座標を返す
	virtual std::array<int, 2> calcTargetIndex() = 0;

protected:
	EnemyComponent&	mEnemy;	//行動パターンを持つエネミー
	DungeonScene&	mScene;	//シーン
};

//ランダムに移動
class RandomWalkStrategy : public EnemyMoveStrategy
{
public:
	RandomWalkStrategy(EnemyComponent& enemy, DungeonScene& scene) : EnemyMoveStrategy(enemy, scene) {}
	std::array<int, 2> calcTargetIndex() override;
};

//A*アルゴリズムでプレイヤーを追跡
class AstarStrategy : public EnemyMoveStrategy
{
public:
	AstarStrategy(EnemyComponent& enemy, DungeonScene& scene) : EnemyMoveStrategy(enemy, scene) {}
	std::array<int, 2> calcTargetIndex() override;

private:
	//A*アルゴリズム用構造体
	struct Node {
		int x, y;
		int fCost;

		bool operator>(const Node& other)const {
			return fCost > other.fCost;
		}
	};
	struct Cell {
		int gCost = 999999; //初期値は大きな値にする
		bool isClosed = false; //探索済みか判定

		//親ノードの位置
		int parentX = -1;
		int parentY = -1;

	};
};

//索敵範囲内にプレイヤーがいればA*で追跡し、いなければランダムに移動
class SenseStrategy : public EnemyMoveStrategy
{
public:
	SenseStrategy(EnemyComponent& enemy, DungeonScene& scene, int senseRange)
		: EnemyMoveStrategy(enemy, scene), mSenseRange(senseRange), mAstar(enemy, scene), mRandomWalk(enemy, scene) {}
	std::array<int, 2> calcTargetIndex() override;

private:
	int					mSenseRange;	//プレイヤーを感知する範囲(マンハッタン距離)
	AstarStrategy		mAstar;			//索敵範囲内での行動
	RandomWalkStrategy	mRandomWalk;	//索敵範囲外での行動
};

//ボス(柱)の行動
//一定ダメージを受けるか一定ターン滞在すると移動し、移動時と一定ターンごとに岩を召喚する
class BossPillarStrategy : public EnemyMoveStrategy
{
public:
	BossPillarStrategy(EnemyComponent& enemy, DungeonScene& scene);
	std::array<int, 2> calcTargetIndex() override;
	
private:
	//ボスの位置ごとの設定
	struct BossPosition {
		std::array<int, 2> position;							//ボスの位置
		std::array<std::array<int, 2>, 2> moveSummonPositions;	//この位置に移動したときの召喚位置
		std::array<int, 2> turnSummonPosition;					//この位置にいるときのターンごとの召喚位置
	};

	std::array<BossPosition, 3> mBossPositions;	//移動可能位置と召喚位置
	int mCurrentMovablePositionIndex;
	int mAccumulatedDamage;
	int mDamageThreshold;
	int mStayedTurnCount;
	int mStayedTurnThreshold;
	
	int mPreEnemyHP;

	//---召喚---
	std::string mSummonEnemyID;	//召喚するエネミーのID
	int mMaxSummonCount;		//召喚したエネミーがフィールドに存在できる上限
	int mSummonTurnCount;		//ターンごとの召喚用のカウンタ
	int mSummonInterval;		//ターンごとの召喚の間隔(ターン)
	
	std::array<int, 2> selectNextPosition();
	void summonOnMove();	//移動先の周囲に召喚
	void summonOnTurn();	//現在地に応じた決まった座標に召喚
};

//エネミーデータ(json)のmovePatternから行動パターンを生成する
std::unique_ptr<EnemyMoveStrategy> createMoveStrategy(const nlohmann::json& enemyJson, EnemyComponent& enemy, DungeonScene& scene);
