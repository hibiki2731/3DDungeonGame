#include "PlayerInteractComponent.h"
#include "Player.h"
#include "CharacterComponent.h"
#include "DungeonScene.h"
#include "Game.h"
#include "AudioManager.h"
#include "ItemManager.h"
#include "Resource.h"
#include "Treasure.h"
#include "SceneManager.h"

PlayerInteractComponent::PlayerInteractComponent(DungeonScene& scene, Player& player, CharacterComponent& character)
	: Component(player),
	mScene(scene),
	mPlayer(player),
	mCharacter(character)
{
}

bool PlayerInteractComponent::collect()
{
	//プレイヤーの位置に資源があるか判定
	auto resource = mScene.getResource(mCharacter.getIndexPosInt());
	//ない場合は何もしない
	if (resource == nullptr) return false;

	//Resourceクラスのポインタからリソースを回収する関数を実行
	resource->collect();

	//SE
	mScene.getGame().getAudioManager().playSE("PICKAXE");
	//メッセージを追加 resourceをx取得した！(現在の総数)
	std::string message = resource->getResourceName() + "を" + std::to_string(resource->getAmount()) + "取得した! (総数"
						 + std::to_string(mScene.getGame().getItemManager().getResourceNum(resource->getResourceID())) + ")\n";
	mScene.pushMessage(message);

	//ターン経過
	mPlayer.endAction();

	return true;
}

bool PlayerInteractComponent::openTreasure()
{
	//前方に宝箱があるか判定
	const std::array<int, 2> searchPos = getFrontIndexPos();
	if (mScene.getTileDataAt(searchPos[0], searchPos[1]) != TileType::TREASURE) return false;

	//宝箱のポインタを取得、設定されていなかったら何もしない
	Treasure* treasure = mScene.getTreasureAt(searchPos[0], searchPos[1]);
	if (treasure == nullptr) return false;

	//宝箱からアイテムを取得
	treasure->open();

	//ターン経過
	mPlayer.endAction();

	return true;
}

bool PlayerInteractComponent::downStairs(){
	const std::array<int, 2> searchPos = getFrontIndexPos();
	if(mScene.getTileDataAt(searchPos[0], searchPos[1]) != TileType::GOAL || mScene.getName() != "DUNGEON_1")
		return false;
	   
	auto& sceneManager = mScene.getGame().getSceneManager();
	sceneManager.transitToMap(2);

	return true;
	
}

bool PlayerInteractComponent::isFacingGoal()
{
	const std::array<int, 2> searchPos = getFrontIndexPos();
	return mScene.getTileDataAt(searchPos[0], searchPos[1]) == TileType::GOAL;
}

std::array<int, 2> PlayerInteractComponent::getFrontIndexPos()
{
	std::array<int, 2> pos = mCharacter.getIndexPos();

	switch (mCharacter.getDirection()) {
	case Direction::UP:
		pos[1] += 1;
		break;
	case Direction::DOWN:
		pos[1] -= 1;
		break;
	case Direction::RIGHT:
		pos[0] += 1;
		break;
	case Direction::LEFT:
		pos[0] -= 1;
		break;
	}

	return pos;
}
