#include "MapManager.h"
#include "RockObject.h"
#include "Game.h"
#include "Definition.h"
#include "Slime.h"
#include <fstream>
#include <cassert>

MapManager::MapManager(const std::shared_ptr<Game>& game)
{
	mMapData.clear();
	mMapSize = 0;
	mGame = game;
	mStage = MAP1;
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

void MapManager::loadMap(Stage stage)
{
	std::ifstream file;

	//ファイルの読み込み
	switch (stage) {
	case MAP1:
		file.open("assets\\mapdata\\stage1.txt");
		break;
	}

	assert(!file.fail());
	file >> mMapSize;

	//マップデータの読み込み
	mMapData.resize(mMapSize * mMapSize);
	for (int y = 0; y < mMapSize; y++)
	{
		for (int x = 0; x < mMapSize; x++)
		{
			int tileNum = 0;
			file >> tileNum;
			mMapData[y * mMapSize + x] = tileNum;
		}
	}
	//オブジェクトデータの読み込み
	mObjectData.resize(mMapSize* mMapSize);
	for (int y = 0; y < mMapSize; y++)
	{
		for (int x = 0; x < mMapSize; x++)
		{
			int objectNum = 0;
			file >> objectNum;
			mObjectData[y * mMapSize + x] = objectNum;
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
			int tileNum = mMapData[y * mMapSize + x];
			if (tileNum == TileType::WALL) continue; //壁の中
			//床の生成
			std::shared_ptr<RockFloor> rockFloor = createActor<RockFloor>(mGame);
			rockFloor->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1)))); //z軸は奥から手前に配置

			//壁の生成
			//西壁
			if (x == 0) {
				auto wall = createActor<RockWall>(mGame);
				wall->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1))));
				wall->setYRot(XM_PIDIV2); 
			} else if(mMapData[y * mMapSize + (x - 1)] == 0) {
				auto wall = createActor<RockWall>(mGame);
				wall->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1))));
				wall->setYRot(XM_PIDIV2); 
			}
			//東壁
			if (x == mMapSize - 1) {
				auto wall = createActor<RockWall>(mGame);
				wall->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1))));
				wall->setYRot(-XM_PIDIV2);
			}
			else if (mMapData[y * mMapSize + (x + 1)] == 0) {
				auto wall = createActor<RockWall>(mGame);
				wall->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1))));
				wall->setYRot(-XM_PIDIV2);
			}
			//北壁
			if (y == 0) {
				auto wall = createActor<RockWall>(mGame);
				wall->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1))));
				wall->setYRot(XM_PI);
			}
			else if (mMapData[(y - 1) * mMapSize + x] == 0) {
				auto wall = createActor<RockWall>(mGame);
				wall->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1))));
				wall->setYRot(XM_PI);
			}
			//南壁
			if (y == mMapSize - 1) {
				auto wall = createActor<RockWall>(mGame);
				wall->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1))));
			}
			else if (mMapData[(y + 1) * mMapSize + x] == 0) {
				auto wall = createActor<RockWall>(mGame);
				wall->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1))));
			}


		}
	}
}

void MapManager::createObject()
{
	int objectNum = 0;
	for (int y = 0; y < mMapSize; y++){
		for (int x = 0; x < mMapSize; x++) {
			objectNum = mObjectData[y * mMapSize + x];

			switch (objectNum) {
			case ObjectType::EMPTY:
				break;
			case ObjectType::SLIME:
				//スライムの生成
				std::shared_ptr<Slime> slime = createActor<Slime>(mGame);
				slime->setPosition(XMFLOAT3((float)(MAPTIPSIZE * x), 0.0f, (float)(MAPTIPSIZE * (mMapSize - y - 1))));
				break;
			}

		}
	}
}
