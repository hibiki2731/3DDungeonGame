#include "EnemySpawner.h"
#include "DungeonScene.h"
#include "EnemyComponent.h"
#include "Player.h"
#include "Random.h"
#include "MapGenerator.h"

EnemySpawner::EnemySpawner(DungeonScene& scene)
	: mScene(scene)
{
}

void EnemySpawner::requestSpawn(const std::string& enemyID, const std::array<int, 2>& indexPos, int maxCount)
{
	mRequests.push_back({ enemyID, indexPos, maxCount });
}

void EnemySpawner::flush()
{
	for (const auto& request : mRequests) {
		//召喚先のマスが埋まっている場合はスキップ
		if (!isSpawnable(request.indexPos[0], request.indexPos[1])) continue;
		//同じIDのエネミーが上限まで存在する場合はスキップ
		if (countEnemies(request.enemyID) >= request.maxCount) continue;

		//エネミーの生成
		mScene.createEnemy(request.enemyID, static_cast<float>(MAPTIPSIZE * request.indexPos[0]), static_cast<float>(MAPTIPSIZE * request.indexPos[1]));
	}

	mRequests.clear();
}

void EnemySpawner::spawnNaturally()
{
	//一定確率で自然湧きする
	if (Random::dist(1, 100) > 8) return;

	//ステージ毎の自然湧きする敵
	std::string spawnEnemyID = "";
	switch (mScene.getStage()) {
		case Stage::MAP1:
			spawnEnemyID = "SLIME";
			break;
		case Stage::MAP2:
			//敵を自然湧きさせない。
			return;
	}

	const std::array<int, 2> playerIndex = mScene.getPlayer()->getIndexPos();

	int i = 0; //湧き場がない場合、一定回数のループ後にループを抜ける

	//障害物がない　かつ　プレイヤーから4マス以上9マス以下の範囲でスポーン
	while (i < 50) {
		//スポーンするマスを乱数で決定
		int x = playerIndex[0] + Random::dist(-9, 9);
		int y = playerIndex[1] + Random::dist(-9, 9);

		//障害物がある場合、もう一度乱数を振りなおす
		if (!isSpawnable(x, y)) {
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
		mScene.createEnemy(spawnEnemyID, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
		break;
	}
}

void EnemySpawner::clear()
{
	mRequests.clear();
}

bool EnemySpawner::isSpawnable(int x, int y)
{
	return mScene.getTileDataAt(x, y) == TileType::FLOOR &&
		mScene.getCharacterDataAt(x, y) == CharacterType::EMPTY;
}

int EnemySpawner::countEnemies(const std::string& enemyID)
{
	int count = 0;
	for (auto enemy : mScene.getEnemies()) {
		if (enemy->getEnemyID() == enemyID) count++;
	}
	return count;
}
