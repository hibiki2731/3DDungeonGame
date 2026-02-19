#pragma once
#include <vector>
#include <memory>
#include "Definition.h"

class Game;

enum class Stage {
	MAP1,
};
struct TileType {
	enum Type {
		WALL = 0,
		FLOOR = 1,
	};
};

struct ObjectType {
	enum Type {
		EMPTY = 0,
		PLAYER = 1,
		SLIME = 2,
	};
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
	TurnType getTurnType();

	//É^Å[Éìêßå‰
	void moveToPlayerTurn();
	void moveToEnemyTurn();

private:
	void loadMap(Stage stage);
	void createWall();
	void createObject();

	TurnType mTurnType;
	std::vector<std::vector<int>> mMapData; //[x][y]
	std::vector<std::vector<int>> mObjectData; //[x][y]
	int mMapSize;
	Stage mStage;
	std::shared_ptr<Game> mGame;
};

