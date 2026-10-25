#include "EnemyAttackStrategy.h"
#include "EnemyComponent.h"
#include "DungeonScene.h"
#include "Player.h"
#include "Game.h"
#include "PlayerManager.h"
#include <functional>
#include <unordered_map>
#include <cassert>

//---近接攻撃---
bool MeleeAttackStrategy::tryAttack()
{
	//プレイヤーのインデックス座標を取得
	Player* player = mScene.getPlayer();
	const std::array<int, 2> playerIndexPos = player->getIndexPos();
	const std::array<int, 2>& indexPos = mEnemy.getIndexPos();

	//プレイヤーが上下左右に隣接していなければ攻撃しない
	const int distance = abs(playerIndexPos[0] - indexPos[0]) + abs(playerIndexPos[1] - indexPos[1]);
	if (distance != 1) return false;

	//プレイヤーの方を向く
	mEnemy.lookAt(playerIndexPos);

	//ダメージを計算し、プレイヤーのHPを減らす
	const PlayerData& playerData = mScene.getGame().getPlayerManager().getPlayerData();
	int damage = max(0, mEnemy.getPower() - playerData.defence);
	player->giveDamage(damage);

	return true;
}

//---攻撃パターンの生成---
std::unique_ptr<EnemyAttackStrategy> createAttackStrategy(const nlohmann::json& enemyJson, EnemyComponent& enemy, DungeonScene& scene)
{
	using Creator = std::function<std::unique_ptr<EnemyAttackStrategy>(const nlohmann::json&, EnemyComponent&, DungeonScene&)>;

	//attackPatternの名前と生成処理の対応表。攻撃パターンを追加する場合はここに登録する
	static const std::unordered_map<std::string, Creator> creators = {
		{ "MELEE",	[](const nlohmann::json&, EnemyComponent& e, DungeonScene& s) { return std::make_unique<MeleeAttackStrategy>(e, s); } },
		{ "NONE",	[](const nlohmann::json&, EnemyComponent& e, DungeonScene& s) { return std::make_unique<NoAttackStrategy>(e, s); } },
	};

	const std::string pattern = enemyJson.at("attackPattern").get<std::string>();
	auto iter = creators.find(pattern);
	assert(iter != creators.end() && "未登録のattackPatternです");
	return iter->second(enemyJson, enemy, scene);
}
