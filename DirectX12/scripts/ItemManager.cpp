#include "ItemManager.h"

ItemManager::ItemManager()
{
	//各アイテムの所持数を0に
	mItemData[Item::GRASS] = 0;
}

ItemManager::~ItemManager()
{
}

void ItemManager::addItem(Item itemName, int num)
{
	mItemData[itemName] += num;
}

void ItemManager::subItem(Item itemName, int num)
{
	mItemData[itemName] -= num;
}

int ItemManager::getItemNum(Item itemName) {
	return mItemData[itemName];
}
