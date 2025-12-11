#include "MessageWindow.h"
#include "TextComponent.h"

void MessageWindow::initActor()
{
	auto text = createComponent<TextComponent>(shared_from_this());
	text->setText(L"これはメッセージウィンドウです。");
	text->setBaseLine(100.0f, 400.0f);
	text->setFontSize(24.0f);
	text->showText();
}

void MessageWindow::inputActor()
{
}

void MessageWindow::updateActor()
{
}
