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
	SLIME = 1,
};

class MapManager
{
public:
	MapManager(const std::shared_ptr<Game>& game);
	~MapManager() {};

	void createMap();
	
	void setStage(Stage stage);

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

