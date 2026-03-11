#pragma once
#include "TownManager.h"

class PlayerManager;

class EquipWeaponMenu : public Menu {
public:
	EquipWeaponMenu(Game* game, float zDepth);
	void updateMenu() override;
private:
	PlayerManager* mPlayerManager;
};

class EquipArmerMenu : public Menu {
public:
	EquipArmerMenu(Game* game, float zDepth);
	void updateMenu() override;

private:
	PlayerManager* mPlayerManager;
};
class StatusMenu : public Menu
{
public:
	StatusMenu(Game* game, float zDepth);
	~StatusMenu();
	void updateMenu() override;

};

