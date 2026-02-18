#include "MessageWindow.h"
#include "TextComponent.h"
#include "SpriteComponent.h"
#include "Graphic.h"
#include "Game.h"
#include "Player.h"

void MessageWindow::initActor()
{
	std::wstring message = L"初期化";
	mMessage = message;
	mText = createComponent<TextComponent>(shared_from_this());
	mText->setText(mMessage);
	mText->setBaseLine(100.0f, 10.0f);
	mText->setFontSize(24.0f);
	mText->setTextColor(D2D1::ColorF(D2D1::ColorF::Black));
	mText->showText();

	auto window = createComponent<SpriteComponent>(shared_from_this());
	window->create("assets\\Default\\Panel\\panel-001.png");
	window->setBordarSize(12.0f);
	window->setSpriteSize(XMFLOAT2(600.0f, 100.0f));

}

void MessageWindow::inputActor()
{
}

void MessageWindow::updateActor()
{
	//デバッグ用
	std::wstring message = L"x:" + std::to_wstring(mPlayer->getPosition().x) +
		L" y:" + std::to_wstring(mPlayer->getPosition().y) +
		L" z:" + std::to_wstring(mPlayer->getPosition().z) + L"\n";

	switch (mPlayer->getDirection()) {
	case Direction::UP:
		message += L"UP";
		break;
	case Direction::DOWN:
		message += L"DOWN";
		break;
	case Direction::RIGHT:
		message += L"RIGHT";
		break;
	case Direction::LEFT:
		message += L"LEFT";
		break;
	}

	mMessage = message;
	mText->setText(mMessage);
}

void MessageWindow::setTarget(const std::shared_ptr<Actor>& actor)
{
	mTarget = actor;
}

void MessageWindow::setPlayer(const std::shared_ptr<Player>& player)
{
	mPlayer = player;
}
