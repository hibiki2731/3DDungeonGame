#include "StatusMenu.h"
#include "Game.h"
#include "TownManager.h"
#include "MessageWindow.h"
#include <memory>
#include "PlayerManager.h"

EquipWeaponMenu::EquipWeaponMenu(Game* game, float zDepth) : Menu(game, "EquipWeaponMenu", zDepth)
{
	mPlayerManager = game->getPlayerManager();
	mMaxIndex = mPlayerManager->getPlayerData().weaponInventory.size();
}

void EquipWeaponMenu::updateMenu()
{
	mPlayerManager->equipWeapon(mSelectedIndex);
}

EquipArmerMenu::EquipArmerMenu(Game* game, float zDepth) : Menu(game, "EquipArmerMenu", zDepth)
{
	mPlayerManager = game->getPlayerManager();
	mMaxIndex = mPlayerManager->getPlayerData().armerInventory.size();
}

void EquipArmerMenu::updateMenu()
{
	mPlayerManager->equipArmer(mSelectedIndex);
}

StatusMenu::StatusMenu(Game* game, float zDepth) : Menu(game, "StatusMenu", zDepth)
{
	mMaxIndex = 2;
}

StatusMenu::~StatusMenu()
{
	mGame->getTownManager()->exitStatusMenu();
}

void StatusMenu::updateMenu()
{
	switch(mSelectedIndex) {
	case 0: {
		auto weaponMenu = std::make_unique<EquipWeaponMenu>(mGame, 48.0f);
		mGame->addActor(std::move(weaponMenu));
		break;
	}
	case 1: {
		auto armerMenu = std::make_unique<EquipArmerMenu>(mGame, 48.0f);
		mGame->addActor(std::move(armerMenu));
		break;
	}
	}
}

