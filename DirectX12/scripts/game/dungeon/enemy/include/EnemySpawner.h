#pragma once
#include <array>
#include <string>
#include <vector>

class DungeonScene;

//エネミーの生成を管理するクラス
//召喚要求を積んでおき、敵配列を走査していない安全なタイミング(ターン終了時)にまとめて生成する
class EnemySpawner
{
public:
	explicit EnemySpawner(DungeonScene& scene);

	//召喚要求を積む。maxCount: 同じIDのエネミーがフィールドに存在できる上限
	void requestSpawn(const std::string& enemyID, const std::array<int, 2>& indexPos, int maxCount);
	//積まれた召喚要求をまとめて生成する。積まれた順に処理するため、先に積んだ要求が優先される
	void flush();
	//ステージごとの自然湧き
	void spawnNaturally();
	//召喚要求を破棄する
	void clear();

private:
	//床 かつ キャラクターがいないマスか判定
	bool isSpawnable(int x, int y);
	//フィールド上の同じIDのエネミーの数を数える
	int countEnemies(const std::string& enemyID);

	//召喚要求
	struct Request {
		std::string enemyID;
		std::array<int, 2> indexPos;
		int maxCount;
	};

	std::vector<Request>	mRequests;	//召喚要求の配列
	DungeonScene&			mScene;		//シーン
};
