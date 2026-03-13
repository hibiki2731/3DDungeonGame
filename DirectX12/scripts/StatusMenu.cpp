#include "StatusMenu.h"
#include "Game.h"
#include "TownManager.h"
#include "MessageWindow.h"
#include <memory>
#include "PlayerManager.h"
#include "TextComponent.h"
#include "MyUtility.h"

EquipWeaponMenu::EquipWeaponMenu(Game* game, float zDepth) : Menu(game, "EquipWeaponMenu", zDepth)
{
	mPlayerManager = game->getPlayerManager();
	mMaxIndex = mPlayerManager->getPlayerData().weaponInventory.size();

	auto weaponText = std::make_unique<TextComponent>(this, zDepth - 0.5f);
	std::wstring message;
	const auto& inventory = mPlayerManager->getPlayerData().weaponInventory;
	for (std::string weapon : inventory) {
		message += Utility::stringToWString(weapon) + L"\n";
	}
	float fontSize =40.0f;
	float lineSpace = 8.0f;
	weaponText->setText(message);
	weaponText->setBaseLine(80.0f, 175.0f);
	weaponText->setFontSize(fontSize);
	weaponText->setTextColor(D2D1::ColorF(D2D1::ColorF::Black));
	weaponText->setLineSpace(lineSpace);
	weaponText->showText();
	mTextComponent = weaponText.get();
	addComponent(std::move(weaponText));

	mArrowMoveLength = fontSize + lineSpace;

}

void EquipWeaponMenu::selectedAct()
{
	mPlayerManager->equipWeapon(mSelectedIndex);
}

void EquipWeaponMenu::updateMenu()
{
	if (mSelectedIndex > MaxShowWeaponNum) {		

	}
}

EquipArmerMenu::EquipArmerMenu(Game* game, float zDepth) : Menu(game, "EquipArmerMenu", zDepth)
{
	mPlayerManager = game->getPlayerManager();
	mMaxIndex = mPlayerManager->getPlayerData().armerInventory.size();

	auto armerText = std::make_unique<TextComponent>(this, zDepth - 0.5f);
	std::wstring message;
	const auto& inventory = mPlayerManager->getPlayerData().armerInventory;
	for (std::string armer : inventory) {
		message += Utility::stringToWString(armer) + L"\n";
	}
	float fontSize =40.0f;
	float lineSpace = 8.0f;
	armerText->setText(message);
	armerText->setBaseLine(80.0f, 175.0f);
	armerText->setFontSize(fontSize);
	armerText->setTextColor(D2D1::ColorF(D2D1::ColorF::Black));
	armerText->setLineSpace(lineSpace);
	armerText->showText();
	addComponent(std::move(armerText));

	mArrowMoveLength = fontSize + lineSpace;
}

void EquipArmerMenu::selectedAct()
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

void StatusMenu::selectedAct()
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

