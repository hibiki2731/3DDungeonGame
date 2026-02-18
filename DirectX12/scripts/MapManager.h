#pragma once
#include <vector>
#include <memory>

class Game;

enum Stage {
	MAP1,
};

enum TileType {
	WALL = 0,
	FLOOR = 1,
};

enum ObjectType {
	EMPTY = 0,
	PLAYER = 1,
	SLIME = 2,
};

class MapManager
{
public:
	MapManager(const std::shared_ptr<Game>& game);
	~MapManager() {};

	void createMap();
	
	//setter
	void setStage(Stage stage);
	void setMapDataAt(int x, int y, int data);
	void setMapDataAt(int index, int data);
	void setObjectDataAt(int x, int y, int data);
	void setObjectDataAt(int index, int data);

	//getter
	int getMapSize();
	int getMapDataAt(int x, int y);
	int getMapDataAt(int index);
	int getObjectDataAt(int x, int y);
	int getObjectDataAt(int index);

private:
	void loadMap(Stage stage);
	void createWall();
	void createObject();

	std::vector<int> mMapData;
	std::vector<int> mObjectData;
	int mMapSize;
	Stage mStage;
	std::shared_ptr<Game> mGame;
};

