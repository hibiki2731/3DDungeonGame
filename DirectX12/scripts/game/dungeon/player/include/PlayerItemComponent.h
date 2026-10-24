#pragma once
#include "Component.h"

class DungeonScene;
class Player;
class CharacterComponent;
class PlayerManager;
class ItemManager;

//プレイヤーのアイテム選択・使用を行うコンポーネント
class PlayerItemComponent : public Component
{
public:
	PlayerItemComponent(DungeonScene& scene, Player& player, CharacterComponent& character);
	DECLARE_COMPONENT_NAME(PlayerItemComponent)

	//選択中のアイテムを使用する(即時に終わる行動のため、使用したらそのまま行動終了する)
	void useItem();
	//アイテム選択カーソルの移動
	void selectNextItem();
	void selectPreviousItem();

	//getter
	int getSelectItemIndex() const { return mSelectItemIndex; }
	const std::string& getSelectItemID();

private:
	DungeonScene& mScene;
	Player& mPlayer;
	CharacterComponent& mCharacter;
	PlayerManager& mPlayerManager;
	ItemManager& mItemManager;

	int mSelectItemIndex;	//現在選択しているアイテムのインデックス
};
