#include "MessageWindow.h"
#include "TextComponent.h"
#include "SpriteComponent.h"
#include "Graphic.h"
#include "Game.h"


void MessageWindow::initActor()
{
	std::wstring message = L"‰Šú‰»";
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
	window->setSpriteSize(XMFLOAT2(600.0f, 50.0f));

}

void MessageWindow::inputActor()
{
}

void MessageWindow::updateActor()
{
	std::wstring message = L"x:" + std::to_wstring(mActor->getPosition().x) +
		L" y:" + std::to_wstring(mActor->getPosition().y) +
		L" z:" + std::to_wstring(mActor->getPosition().z);

	mMessage = message;
	mText->setText(mMessage);
}

void MessageWindow::setActor(const std::shared_ptr<Actor>& actor)
{
	mActor = actor;
}
