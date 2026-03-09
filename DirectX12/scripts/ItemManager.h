#pragma once
#include <unordered_map>
#include <string>

class ItemManager
{
public:
	ItemManager();

	void addResource(std::string id, int num);
	void subResource(std::string id, int num);

	int getResourceNum(std::string id);
	std::unordered_map<std::string, size_t>& getResourceData();

private:
	std::unordered_map<std::string, size_t> mResourceData;
};

