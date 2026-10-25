#pragma once
#include "Scene.h"
#include <memory>
#include <unordered_map>

class Player;
class EnemyComponent;
class FireParticleComponent;
class Treasure;
class Object;

enum class Stage {
	MAP1,
	MAP2,
	COUNT
};
struct TileType {
	enum Type {
		GOAL = -1,
		WALL = 0,
		FLOOR = 1,
		RESOURCE = 2,
		TREASURE = 3,
		EMPTY = 4,
	};
};

struct CharacterType {
	enum Type {
		EMPTY = 0,
		PLAYER = -1,
		ENEMY = 1,
	};
};

class DungeonScene :
    public Scene
{
public:
    DungeonScene(class Game& game, Stage stage);
    virtual ~DungeonScene();

    void fastUpdateScene() override;
	void updateScene() override;
	void lateUpdateScene() override;
	void drawScene() override;
    void onEnter() override;
	virtual void onEnterMap() {};
    void onExit() override;
	virtual void onExitMap() {};
    
    //シーン名は階層ごとの派生クラスで定義する
    virtual const std::string getName() const override = 0;


    //エネミーの作成
    void createEnemy(const std::string& enemyID, float x, float y);
    //プレイヤーの作成
	void createPlayer(float x, float y);
    //リソースの作成
	void createResource(const std::string& resourceID, const std::string& meshID, float x, float y, int index);
	void spawnResource();
	void deleteResourceFromIndex(int index);

    //シーン遷移
    void returnToTown();
	void transitToGameOver();

    //エネミー配列の制御
	void addEnemy(EnemyComponent* enemy);
	void removeEnemy(EnemyComponent* enemy);
	void sortEnemiesByDistanceToPlayer();

	//パーティクル配列の制御
	void addParticle(FireParticleComponent* particle);
	void removeParticle(FireParticleComponent* particle);

	//ミニマップの更新
	void updateMiniMapPos();
	void updateMiniMapDirection();
	//UIの更新
	void updateHPUI();
	void updateAPUI();
	void updateItemUI();
	void updateItemFrame();
	void pushMessage(const std::string& message);
	void updateGold();

    //setter
    //マップ情報
	void setTileDataAt(int x, int y, int data);
	void setTileDataAt(int index, int data);
	void setTileData(std::vector<std::vector<int>> mapData);
	void setCharacterDataAt(int x, int y, int data);
	void setCharacterDataAt(int index, int data);
	void setCharacterData(std::vector<std::vector<int>> objectData);
	void setMapSize(int size) { mMapSize = size; }
	void setTreasures(std::vector<Treasure*> treasures) { mTreasures = std::move(treasures); }

    //getter
    //マップ情報
	const int getMapSize() const { return mMapSize; }
	int getTileDataAt(int x, int y);
	int getTileDataAt(int index);   
	int getCharacterDataAt(int x, int y);
	int getCharacterDataAt(int index);
	Treasure* getTreasureAt(int x, int y);
	const Stage& getStage();
    //エネミー
	const std::vector<EnemyComponent*>& getEnemies() const { return mEnemies; }
	const int getEnemyCount() const { return mEnemies.size(); }
	EnemyComponent* getEnemyFromIndexPos(int index);
	EnemyComponent* getEnemyFromIndexPos(int x, int y);
    //プレイヤー
	Player* getPlayer() const { return mPlayer; }
    int getPlayerActLimit();
    //リソース
    class Resource* getResource(int index);
    class Resource* getResource(int x, int y);
	//ダメージテキスト
	float getDamageTextNum() const;
	void createDamageText(const XMFLOAT3& pos, int digit);
	//ターンオブザーバー
	class TurnStateMachine& getTurnStateMachine();
	//エネミー生成管理
	class EnemySpawner& getEnemySpawner();

private:
	//マップ情報
    int mMapSize;
	std::vector<std::vector<int>> mTileData; //[x][y]
	std::vector<std::vector<int>> mCharacterData; //[x][y]
	std::vector<Treasure*> mTreasures;
	Stage mStage;

	//キャラクター
	std::vector<EnemyComponent*> mEnemies;
	Player* mPlayer;
	//リソース
	std::unordered_map<int, class Resource*> mResources;
	int mMaxResourcePoint;

	//UIアクター
	class MiniMap* mMiniMap;
	class DungeonUI* mUI;

	//エフェクト
	std::vector<FireParticleComponent*> mParticles;
	
	//管理クラス
	std::unique_ptr<class MapGenerator> mMapGenerator;	//マップ生成
	std::unique_ptr<class TurnStateMachine > mTurnStateMachine;	//ターン管理
	std::unique_ptr<class DamageTextGenerator> mDamageTextManaager;	//ダメージテキスト管理
	std::unique_ptr<class EnemySpawner> mEnemySpawner;	//エネミー生成管理
};

//1階のシーン
class Dungeon1Scene :
	public DungeonScene
{
public:
	Dungeon1Scene(class Game& game) : DungeonScene(game, Stage::MAP1) {}

	const std::string getName() const override {
		return "DUNGEON_1";
	};
};

//2階のシーン
class Dungeon2Scene :
	public DungeonScene
{
public:
	Dungeon2Scene(class Game& game);
	~Dungeon2Scene() override;

	void onEnterMap() override;
	const std::string getName() const override {
		return "DUNGEON_2";
	};

private:
	std::unique_ptr<Object> mBossObj;
};
