#pragma once
#include "TownManager.h"

class PlayerManager;
class TextComponent;

class EquipWeaponMenu : public Menu {
public:
	EquipWeaponMenu(Game* game, float zDepth);
	void selectedAct() override;
	void updateMenu() override;
private:
	PlayerManager* mPlayerManager;
	TextComponent* mTextComponent;

	const int MaxShowWeaponNum = 10;
};

class EquipArmerMenu : public Menu {
public:
	EquipArmerMenu(Game* game, float zDepth);
	void selectedAct() override;

private:
	PlayerManager* mPlayerManager;
	const int MaxShowArmerNum = 10;
};
class StatusMenu : public Menu
{
public:
	StatusMenu(Game* game, float zDepth);
	~StatusMenu();
	void selectedAct() override;

};

