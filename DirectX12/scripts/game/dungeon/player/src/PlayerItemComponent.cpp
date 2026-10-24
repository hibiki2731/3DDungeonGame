#include "PlayerItemComponent.h"
#include "Player.h"
#include "CharacterComponent.h"
#include "DungeonScene.h"
#include "Game.h"
#include "AudioManager.h"
#include "PlayerManager.h"
#include "ItemManager.h"

PlayerItemComponent::PlayerItemComponent(DungeonScene& scene, Player& player, CharacterComponent& character)
	: Component(player),
	mScene(scene),
	mPlayer(player),
	mCharacter(character),
	mPlayerManager(scene.getGame().getPlayerManager()),
	mItemManager(scene.getGame().getItemManager()),
	mSelectItemIndex(0)
{
}

void PlayerItemComponent::useItem()
{
	//選択中のIndexからアイテムのIDを取得
	const auto& itemID = mPlayerManager.getInventoryItem(mSelectItemIndex);
	if (itemID == "") {
		//アイテムがない場合は何もしない
		return;
	}

	//アイテムのIDからアイテムデータを取得
	const ItemData& itemData = mItemManager.getItemData(itemID);

	//アイテムのカテゴリーから効果を発揮
	//HP回復
	if (itemData.category == "HP_RECOVER") {
		mCharacter.addHP(itemData.value);
		mScene.updateHPUI();
		mScene.getGame().getAudioManager().playSE("RECOVER1");
	}
	//AP回復
	else if (itemData.category == "AP_RECOVER") {
		mPlayer.recoverAP(itemData.value);
		mScene.getGame().getAudioManager().playSE("RECOVER1");
	}

	//使用したアイテムをインベントリーから削除
	mPlayerManager.removeInventory(mSelectItemIndex);
	//選択中アイテムを左端に初期化
	mSelectItemIndex = 0;

	//UIの更新
	mScene.updateItemUI();
	mScene.updateItemFrame();

	//ターン経過
	mPlayer.endAction();
}

void PlayerItemComponent::selectNextItem()
{
	//アイテムカーソルを右に移動
	if (mSelectItemIndex >= mPlayerManager.getPlayerData().storageSize - 1) return;
	mSelectItemIndex++;
	mScene.updateItemFrame();
	mScene.getGame().getAudioManager().playSE("UI_MOVE1");
}

void PlayerItemComponent::selectPreviousItem()
{
	//アイテムカーソルを左に移動
	if (mSelectItemIndex <= 0) return;
	mSelectItemIndex--;
	mScene.updateItemFrame();
	mScene.getGame().getAudioManager().playSE("UI_MOVE1");
}

const std::string& PlayerItemComponent::getSelectItemID()
{
	return mPlayerManager.getInventoryItem(mSelectItemIndex);
}
