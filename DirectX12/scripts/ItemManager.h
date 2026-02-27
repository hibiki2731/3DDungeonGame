#pragma once
#include <unordered_map>

enum class Item{
	GRASS,
};

class ItemManager
{
public:
	ItemManager();
	~ItemManager();

	void addItem(Item itemName, int num);
	void subItem(Item itemName, int num);

	int getItemNum(Item itemName);

private:
	std::unordered_map<Item, int> mItemData;
};

