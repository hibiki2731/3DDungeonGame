#pragma once
#include <memory>
#include "json.hpp"

//前方宣言
class EnemyComponent;
class DungeonScene;

//---エネミーの攻撃パターン(ストラテジー)---
//MeleeAttackStrategy	: 上下左右に隣接したプレイヤーを攻撃
//NoAttackStrategy		: 攻撃しない

//攻撃パターンの基底クラス
class EnemyAttackStrategy
{
public:
	EnemyAttackStrategy(EnemyComponent& enemy, DungeonScene& scene) : mEnemy(enemy), mScene(scene) {}
	virtual ~EnemyAttackStrategy() = default;
	//攻撃を試みる。攻撃した(ターンを消費した)場合はtrue、攻撃しなかった場合はfalseを返す
	virtual bool tryAttack() = 0;

protected:
	EnemyComponent&	mEnemy;	//攻撃パターンを持つエネミー
	DungeonScene&	mScene;	//シーン
};

//上下左右に隣接したプレイヤーを攻撃
class MeleeAttackStrategy : public EnemyAttackStrategy
{
public:
	MeleeAttackStrategy(EnemyComponent& enemy, DungeonScene& scene) : EnemyAttackStrategy(enemy, scene) {}
	bool tryAttack() override;
};

//攻撃しない
class NoAttackStrategy : public EnemyAttackStrategy
{
public:
	NoAttackStrategy(EnemyComponent& enemy, DungeonScene& scene) : EnemyAttackStrategy(enemy, scene) {}
	bool tryAttack() override { return false; }
};

//エネミーデータ(json)のattackPatternから攻撃パターンを生成する
std::unique_ptr<EnemyAttackStrategy> createAttackStrategy(const nlohmann::json& enemyJson, EnemyComponent& enemy, DungeonScene& scene);
