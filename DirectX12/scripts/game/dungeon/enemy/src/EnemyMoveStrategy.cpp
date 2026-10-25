#include "EnemyMoveStrategy.h"
#include "EnemyComponent.h"
#include "DungeonScene.h"
#include "Player.h"
#include "Random.h"
#include <queue>
#include <functional>
#include <unordered_map>
#include <cassert>

//---ランダム移動---
std::array<int, 2> RandomWalkStrategy::calcTargetIndex()
{
	std::array<int, 2> targetIndex = mEnemy.getIndexPos();

	//ランダムに移動
	int direction = 1 << Random::dist(0,4);
	switch (direction) {
		case Direction::UP:
			targetIndex[1] += 1;
			if (targetIndex[1] > mScene.getMapSize() - 1) targetIndex[1]--;
			break;
		case Direction::DOWN:
			targetIndex[1] -= 1;
			if (targetIndex[1] < 0) targetIndex[1]++;
			break;
		case Direction::RIGHT:
			targetIndex[0] += 1;
			if (targetIndex[0] > mScene.getMapSize() - 1) targetIndex[0]--;
			break;
		case Direction::LEFT:
			targetIndex[0] -= 1;
			if (targetIndex[0] < 0) targetIndex[0]++;
			break;
		case 1 << 4:
			//その場で停止
			break;
	}

	return targetIndex;
}

//---A*アルゴリズムによる追跡---
std::array<int, 2> AstarStrategy::calcTargetIndex()
{
	const std::array<int, 2>& indexPos = mEnemy.getIndexPos();

	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList; //次に探索するマスのリスト
	std::vector<std::vector<Cell>> grid(mScene.getMapSize(), std::vector<Cell>(mScene.getMapSize()));

	openList.push({ indexPos[0], indexPos[1], 0 }); //スタート地点
	grid[indexPos[0]][indexPos[1]].gCost = 0;
	grid[indexPos[0]][indexPos[1]].isClosed = true;


	const std::array<int, 2> playerIndex = mScene.getPlayer()->getIndexPos();

	while (!openList.empty()) {
		//openListの先頭ノードをカレントノードに
		Node current = openList.top();
		openList.pop(); //先頭から削除

		//クローズドリストの更新
		grid[current.x][current.y].isClosed = true;

		//カレントノードがプレイヤーと同じ座標なら終了
		if (current.x == playerIndex[0] && current.y == playerIndex[1]) {
			break;
		}

		//周囲4マスのコストを計算
		int hCost = 0;
		int gCost = 0;
		int fCost = 0;
		int nextIndex[2] = { 0, 0 };
		for (int direct = 0; direct < 4; direct++) {
			switch (direct) {
			case 0:
				//上
				nextIndex[0] = current.x; nextIndex[1] = current.y + 1;
				break;
			case 1:
				//下
				nextIndex[0] = current.x; nextIndex[1] = current.y - 1;
				break;
			case 2:
				//右
				nextIndex[0] = current.x + 1; nextIndex[1] = current.y;
				break;
			case 3:
				//左
				nextIndex[0] = current.x - 1; nextIndex[1] = current.y;
				break;
			}

			//探索するマスが移動可能かつ探索していないか判定
			if (nextIndex[0] >= 0 && nextIndex[0] < mScene.getMapSize()
				&& nextIndex[1] >= 0 && nextIndex[1] < mScene.getMapSize()
				&& !grid[nextIndex[0]][nextIndex[1]].isClosed
				&& mScene.getTileDataAt(nextIndex[0], nextIndex[1]) != TileType::WALL
				) {
				gCost = grid[current.x][current.y].gCost + 1;
				//よりGコストの小さいルートを見つけた場合、親ノードとGコストを更新
				if (grid[nextIndex[0]][nextIndex[1]].gCost > gCost) {
					grid[nextIndex[0]][nextIndex[1]].parentX = current.x;
					grid[nextIndex[0]][nextIndex[1]].parentY = current.y;
					grid[nextIndex[0]][nextIndex[1]].gCost = gCost;
					hCost = abs(nextIndex[0] - playerIndex[0]) + abs(nextIndex[1] - playerIndex[1]); //プレイヤーまでのマンハッタン距離
					fCost = hCost + gCost;
					openList.push({ nextIndex[0] , nextIndex[1], fCost });
				}
			}
		}
	}

	//プレイヤーにたどり着けない場合その場にとどまる
	if (grid[playerIndex[0]][playerIndex[1]].isClosed == false) return indexPos;
	int childIndex[2];
	childIndex[0] = playerIndex[0];
	childIndex[1] = playerIndex[1];

	//次に進むマスを親ノードから探索
	while (true) {
		int parentIndex[2] = { grid[childIndex[0]][childIndex[1]].parentX, grid[childIndex[0]][childIndex[1]].parentY };
		if (parentIndex[0] == indexPos[0] && parentIndex[1] == indexPos[1]) {
			return { childIndex[0], childIndex[1] };
		}

		childIndex[0] = parentIndex[0]; childIndex[1] = parentIndex[1];
	}
}

//---索敵---
std::array<int, 2> SenseStrategy::calcTargetIndex()
{
	//索敵範囲内プレイヤーがいた場合
	if (mEnemy.getDist() <= mSenseRange) {
		return mAstar.calcTargetIndex();
	}
	//索敵範囲内プレイヤーがいない場合
	else {
		return mRandomWalk.calcTargetIndex();
	}
}

//---ボス(弱点)---
BossPillarStrategy::BossPillarStrategy(EnemyComponent& enemy, DungeonScene& scene) :
	EnemyMoveStrategy(enemy, scene),
	mMovablePosition(
		std::array<int, 2>{3,5},
		std::array<int, 2>{1,2},
		std::array<int, 2>{4,1}),
	mCurrentMovablePositionIndex(0),
	mAccumulatedDamage(0),
	mDamageThreshold(20),
	mStayedTurnCount(0),
	mStayedTurnThreshold(9),
	mPreEnemyHP(-1)
{
}

std::array<int, 2> BossPillarStrategy::calcTargetIndex()
{
	//前ターンのHPの初期化
	if(mPreEnemyHP < 0) mPreEnemyHP = mEnemy.getHP();
	
	//受けたダメージを計算
	const int currentHP = mEnemy.getHP();
	const int damage = mPreEnemyHP - currentHP; 
	
	//蓄積ダメージ値が許容量を超えていたら移動
	mAccumulatedDamage += damage;
	if(mAccumulatedDamage >= mDamageThreshold){
		mAccumulatedDamage = 0;
		mStayedTurnCount = 0;
		return selectNextPosition();	
	}
	
	//滞在ターン数が閾値を超えてきたら移動
	++mStayedTurnCount;
	if(mStayedTurnCount >= mStayedTurnThreshold){
		mAccumulatedDamage = 0;
		mStayedTurnCount = 0;
		return selectNextPosition();
	}
	
	//前ターンのHPを更新
	mPreEnemyHP = currentHP;

	//プレイヤーの方向を向く
	mEnemy.lookAt(mScene.getPlayer()->getIndexPos());
	//現在地を返す
	return mEnemy.getIndexPos();
}

std::array<int, 2> BossPillarStrategy::selectNextPosition()
{
	//移動候補地をランダムに選出
	int nextPositionIndex = Random::dist(0, mMovablePosition.size() - 2);	//そのまま配列のインデックスになるよう-2する
	//現在地以外のインデックスになるよう調整
	if(nextPositionIndex >= mCurrentMovablePositionIndex) ++nextPositionIndex;

	//現在の移動候補地インデックスを更新
	mCurrentMovablePositionIndex = nextPositionIndex;

    return mMovablePosition[nextPositionIndex];
}

//---行動パターンの生成---
std::unique_ptr<EnemyMoveStrategy> createMoveStrategy(const nlohmann::json& enemyJson, EnemyComponent& enemy, DungeonScene& scene)
{
	using Creator = std::function<std::unique_ptr<EnemyMoveStrategy>(const nlohmann::json&, EnemyComponent&, DungeonScene&)>;

	//movePatternの名前と生成処理の対応表。行動パターンを追加する場合はここに登録する
	static const std::unordered_map<std::string, Creator> creators = {
		{ "RANDOM",			[](const nlohmann::json&,   EnemyComponent& e, DungeonScene& s) { return std::make_unique<RandomWalkStrategy>(e, s); } },
		{ "ASTAR",			[](const nlohmann::json&,   EnemyComponent& e, DungeonScene& s) { return std::make_unique<AstarStrategy>(e, s); } },
		{ "SENSE",			[](const nlohmann::json& j, EnemyComponent& e, DungeonScene& s) { return std::make_unique<SenseStrategy>(e, s, j.value("senseRange", 0)); } },
		{ "BOSS_PILLAR",	[](const nlohmann::json&,   EnemyComponent& e, DungeonScene& s) { return std::make_unique<BossPillarStrategy>(e, s); } },
	};

	const std::string pattern = enemyJson.at("movePattern").get<std::string>();
	auto iter = creators.find(pattern);
	assert(iter != creators.end() && "未登録のmovePatternです");
	return iter->second(enemyJson, enemy, scene);
}
