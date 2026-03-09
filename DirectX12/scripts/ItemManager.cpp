#include "ItemManager.h"
#include "json.hpp"
#include <fstream>

ItemManager::ItemManager()
{
	std::fstream file("assets/data/ItemData.json");
	assert(!file.fail());

	nlohmann::json json;
	file >> json;
	//保存してあるリソース数を読み込む
	for (auto& resourceJson : json["Resource"]) {
		mResourceData[resourceJson["id"]] = resourceJson["num"];
	}
}

void ItemManager::addResource(std::string id, int num)
{
	mResourceData[id] += static_cast<size_t>(num);
}

void ItemManager::subResource(std::string id, int num)
{
	int value = static_cast<int>(mResourceData[id]) - num;
	if (value < 0) return; //数が負の数になる場合、0で止める

	mResourceData[id] = static_cast<size_t>(value);
}

int ItemManager::getResourceNum(std::string id) {
	return static_cast<int>(mResourceData[id]);
}

std::unordered_map<std::string, size_t>& ItemManager::getResourceData()
{
	return mResourceData;
}
