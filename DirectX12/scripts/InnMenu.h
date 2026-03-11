#pragma once
#include "TownManager.h"

class InnMenu : public Menu {
public:
	InnMenu(Game* game, float zDepth);
	void updateMenu() override;

private:
	void stay();
	void save();
};

