#include "MapManager.h"
#include "RockObject.h"
#include "Game.h"
#include "Definition.h"
#include "Slime.h"
#include "Player.h"
#include "Grass.h"
#include <fstream>
#include <cassert>

MapManager::MapManager(Game* game)
{
	mMapData.clear();
	mMapSize = 1;
	mGame = game;
	mStage = Stage::MAP1;
	mNextTurn = TurnType::PLAYER;
	mTurnType = TurnType::PLAYER;
	mPendingEnemyCount = 0;


}

void MapManager::update()
{
	//エネミーターン→プレイヤーターンへの移行時
	if (mNextTurn == TurnType::PLAYER && mTurnType == TurnType::ENEMY) {
		//敵のランダム湧き
		int random = Random::dist(1, 100);
		if (random <= 10) spawnEnemy();
	}
	mTurnType = mNextTurn;
}

void MapManager::createMap()
{
	loadMap(mStage);	//マップデータの読み込み
	createWall();	//マップの壁、床の生成
	createObject(); //オブジェクトの生成
}

void MapManager::setStage(Stage stage)
{
	mStage = stage;
}

void MapManager::setMapDataAt(int x, int y, int data)
{
	//配列範囲外なら無効
	if (x < 0 || x > mMapSize - 1 ||
		y < 0 || y > mMapSize - 1) return;

	mMapData[x][y] = data;
}

void MapManager::setMapDataAt(int index, int data)
{
	int x = index % mMapSize;
	int y = index / mMapSize;

	//配列範囲外なら無効
	if (x < 0 || x > mMapSize - 1 ||
		y < 0 || y > mMapSize - 1) return;

	mMapData[x][y] = data;
}

void MapManager::setObjectDataAt(int x, int y, int data)
{
	//配列範囲外なら無効
	if (x < 0 || x > mMapSize - 1 ||
		y < 0 || y > mMapSize - 1) return;

	mObjectData[x][y] = data;
}

void MapManager::setObjectDataAt(int index, int data)
{
	int x = index % mMapSize;
	int y = index / mMapSize;

	//配列範囲外なら無効
	if (x < 0 || x > mMapSize - 1 ||
		y < 0 || y > mMapSize - 1) return;

	mObjectData[x][y] = data;
}

int MapManager::getMapSize()
{
	return mMapSize;
}

int MapManager::getMapDataAt(int x, int y)
{
	//配列範囲外なら壁を出力
	if (x < 0 || x > mMapSize - 1 ||
		y < 0 || y > mMapSize - 1) return TileType::WALL;

	return mMapData[x][y];
}

int MapManager::getMapDataAt(int index)
{
	int x = index % mMapSize;
	int y = index / mMapSize;

	//配列範囲外なら壁を出力
	if (x < 0 || x > mMapSize - 1 ||
		y < 0 || y > mMapSize - 1) return TileType::WALL;
	return mMapData[x][y];
}

int MapManager::getObjectDataAt(int x, int y)
{
	//配列範囲外なら空を出力
	if (x < 0 || x > mMapSize - 1 ||
		y < 0 || y > mMapSize - 1) return ObjectType::EMPTY;

	return mObjectData[x][y];
}

int MapManager::getObjectDataAt(int index)
{
	int x = index % mMapSize;
	int y = index / mMapSize;

	//配列範囲外なら空を出力
	if (x < 0 || x > mMapSize - 1 ||
		y < 0 || y > mMapSize - 1) return ObjectType::EMPTY;

	return mObjectData[x][y];
} 

TurnType MapManager::getTurnType()
{
	return mTurnType;
}

void MapManager::moveToPlayerTurn()
{
	mPendingEnemyCount--;
	if (mPendingEnemyCount == 0) mNextTurn = TurnType::PLAYER;

}

void MapManager::moveToEnemyTurn()
{
	mPendingEnemyCount = static_cast<int>(mGame->getEnemies().size()); //待機敵数をリセット
	if (mPendingEnemyCount == 0) return;
	mGame->activateEnemies();
	mNextTurn = TurnType::ENEMY;
}

void MapManager::loadMap(Stage stage)
{
	std::ifstream file;

	//ファイルの読み込み
	switch (stage) {
	case Stage::MAP1:
		file.open("assets\\mapdata\\stage1.txt");
		break;
	}

	assert(!file.fail());
	file >> mMapSize;

	//マップデータの読み込み
	mMapData.resize(mMapSize);
	for (int i = 0; i < mMapSize; i++) mMapData[i].resize(mMapSize);

	for (int y = mMapSize - 1; y >= 0; y--)
	{
		for (int x = 0; x < mMapSize; x++)
		{
			int tileNum = 0;
			file >> tileNum;
			mMapData[x][y] = tileNum;
		}
	}
	//オブジェクトデータの読み込み
	mObjectData.resize(mMapSize);
	for (int i = 0; i < mMapSize; i++) mObjectData[i].resize(mMapSize);

	for (int y = mMapSize - 1; y >= 0; y--)
	{
		for (int x = 0; x < mMapSize; x++)
		{
			int objectNum = 0;
			file >> objectNum;
			mObjectData[x][y] = objectNum;
		}
	}

	file.close();

}

void MapManager::createWall()
{
	for (int y = 0; y < mMapSize; y++)
	{
		for (int x = 0; x < mMapSize; x++)
		{
			int tileNum = mMapData[x][y];
			if (tileNum == TileType::WALL) continue; //壁の中

			switch (tileNum) {
			case TileType::FLOOR: {
				//床の生成
				std::unique_ptr<RockFloor> rockFloor = std::make_unique<RockFloor>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				mGame->addActor(std::move(rockFloor)); //所有権をGameへ渡す
				break;
				}
			case TileType::GRASS: {
				//草の生成
				std::unique_ptr<Grass> grass = std::make_unique<Grass>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				mGame->addActor(std::move(grass)); //所有権をGameへ渡す
				}
			}

			//壁の生成
			//西壁
			if (x == 0) {
				auto wall = std::make_unique<RockWall>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				wall->setYRot(XM_PIDIV2); 
				mGame->addActor(std::move(wall));
			} else if(mMapData[x - 1][y] == TileType::WALL) {
				auto wall = std::make_unique<RockWall>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				wall->setYRot(XM_PIDIV2);
				mGame->addActor(std::move(wall));
			}
			//東壁
			if (x == mMapSize - 1) {
				auto wall = std::make_unique<RockWall>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				wall->setYRot(-XM_PIDIV2);
				mGame->addActor(std::move(wall));
			}
			else if (mMapData[x + 1][y] == TileType::WALL) {
				auto wall = std::make_unique<RockWall>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				wall->setYRot(-XM_PIDIV2);
				mGame->addActor(std::move(wall));
			}
			//北壁
			if (y == mMapSize - 1) {
				auto wall = std::make_unique<RockWall>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				wall->setYRot(XM_PI);
				mGame->addActor(std::move(wall));
			}
			else if (mMapData[x][y + 1] == TileType::WALL) {
				auto wall = std::make_unique<RockWall>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				wall->setYRot(XM_PI);
				mGame->addActor(std::move(wall));
			}
			//南壁
			if (y == 0) {
				auto wall = std::make_unique<RockWall>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				mGame->addActor(std::move(wall));
			}
			else if (mMapData[x][y - 1] == TileType::WALL) {
				auto wall = std::make_unique<RockWall>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
				mGame->addActor(std::move(wall));
			}


		}
	}
}

void MapManager::createObject()
{
	int objectNum = 0;
	for (int y = 0; y < mMapSize; y++){
		for (int x = 0; x < mMapSize; x++) {
			objectNum = mObjectData[x][y];

			switch (objectNum) {
				case ObjectType::EMPTY:
					break;
				case ObjectType::PLAYER: {
					//プレイヤー生成
					std::unique_ptr player = std::make_unique<Player>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
					mGame->addActor(std::move(player)); //所有権をGameへ渡す
					break;
				}
				case ObjectType::SLIME: {
					//スライムの生成
					std::unique_ptr<Slime> slime = std::make_unique<Slime>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
					mGame->addActor(std::move(slime)); //所有権をGameへ渡す
					break;
				}
			}

		}
	}
}

void MapManager::spawnEnemy()
{
	int playerIndex[2];
	mGame->getPlayer()->getIndexPos(playerIndex);

	int i = 0; //湧き場がない場合、一定回数のループ後にループを抜ける

	//障害物がない　かつ　プレイヤーから3マス離れているところにスポーン
	while (i < 10) {
		//スポーンするマスを乱数で決定
		int x = Random::dist(0, mMapSize - 1);
		int y = Random::dist(0, mMapSize - 1);

		//障害物がある場合、もう一度乱数を振りなおす
		if (mMapData[x][y] == TileType::WALL) continue;
		if (mObjectData[x][y] != ObjectType::EMPTY) continue;
		
		//プレイヤーから3マスいないならば、もう一度乱数を振りなおす
		int distance = abs(playerIndex[0] - x) + abs(playerIndex[1] - y);
		if (distance <= 3) continue;

		//敵の生成
		std::unique_ptr<Slime> slime = std::make_unique<Slime>(mGame, static_cast<float>(MAPTIPSIZE * x), static_cast<float>(MAPTIPSIZE * y));
		mGame->addActor(std::move(slime)); //所有権をGameへ渡す
		break;

		i++;
	}
}
