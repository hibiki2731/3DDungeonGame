#pragma once
#include <memory>
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
	virtual ~EnemyMoveStrategy() = default;
	//移動先のインデックス座標を計算する。targetIndexには現在のインデックス座標が入った状態で渡される
	virtual std::array<int, 2> calcTargetIndex(EnemyComponent& enemy, DungeonScene& scene) = 0;
};

//ランダムに移動
class RandomWalkStrategy : public EnemyMoveStrategy
{
public:
	std::array<int, 2> calcTargetIndex(EnemyComponent& enemy, DungeonScene& scene) override;
};

//A*アルゴリズムでプレイヤーを追跡
class AstarStrategy : public EnemyMoveStrategy
{
public:
	std::array<int, 2> calcTargetIndex(EnemyComponent& enemy, DungeonScene& scene) override;

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
	explicit SenseStrategy(int senseRange) : mSenseRange(senseRange) {}
	std::array<int, 2> calcTargetIndex(EnemyComponent& enemy, DungeonScene& scene);

private:
	int					mSenseRange;	//プレイヤーを感知する範囲(マンハッタン距離)
	AstarStrategy		mAstar;			//索敵範囲内での行動
	RandomWalkStrategy	mRandomWalk;	//索敵範囲外での行動
};

//ボス(柱)の行動
class BossPillarStrategy : public EnemyMoveStrategy
{
public:
	//TODO: ボスの挙動を実装する。現在はその場から動かない
	std::array<int, 2> calcTargetIndex(EnemyComponent& enemy, DungeonScene& scene);
};

//エネミーデータ(json)のmovePatternから行動パターンを生成する
std::unique_ptr<EnemyMoveStrategy> createMoveStrategy(const nlohmann::json& enemyJson);
