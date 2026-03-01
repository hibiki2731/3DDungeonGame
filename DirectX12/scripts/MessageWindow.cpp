#include "MessageWindow.h"
#include "TextComponent.h"
#include "SpriteComponent.h"
#include "Graphic.h"
#include "Game.h"
#include "Player.h"
#include "MapManager.h"

MessageWindow::MessageWindow(Game* game) : Actor(game)
{

	std::wstring message = L"初期化";
	mMessage = message;
	auto text = std::make_unique<TextComponent>(this);
	text->setText(mMessage);
	text->setBaseLine(100.0f, 10.0f);
	text->setFontSize(24.0f);
	text->setTextColor(D2D1::ColorF(D2D1::ColorF::Black));
	text->showText();
	mText = text.get();
	addComponent(std::move(text));

	auto window = std::make_unique<SpriteComponent>(this);
	window->create("assets\\Default\\Panel\\panel-001.png");
	window->setBordarSize(12.0f);
	window->setSpriteSize(XMFLOAT2(600.0f, 100.0f));
	addComponent(std::move(window));
}

void MessageWindow::inputActor()
{
}

void MessageWindow::updateActor()
{
	if (mPlayer == nullptr) return;
	//デバッグ用
	std::wstring message = L"x:" + std::to_wstring(mPlayer->getPosition().x) +
		L" y:" + std::to_wstring(mPlayer->getPosition().y) +
		L" z:" + std::to_wstring(mPlayer->getPosition().z) + L"\n";
	message += L"TURN: ";

	switch (mGame->getMapManager()->getTurnType()) {
	case TurnType::PLAYER:
		message += L"PLAYER ";
		break;
	case TurnType::ENEMY:
		message += L"ENEMY ";
		break;
	}

	message += L"HP: " + std::to_wstring(mPlayer->getHP()) + L" ";

	message += L"G:" + std::to_wstring(mGame->getItemManager()->getItemNum(Item::GRASS)) + L".";

	mMessage = message;
	mText->setText(mMessage);
}

void MessageWindow::setTarget(Actor* actor)
{
	mTarget = actor;
}

void MessageWindow::setPlayer(Player* player)
{
	mPlayer = player;
}
