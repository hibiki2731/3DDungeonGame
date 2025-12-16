#include "MessageWindow.h"
#include "TextComponent.h"
#include "Camera.h"

void MessageWindow::initActor()
{
	std::wstring message = L"‰Šú‰»";
	mMessage = message;
	mText = createComponent<TextComponent>(shared_from_this());
	mText->setText(mMessage);
	mText->setBaseLine(100.0f, 400.0f);
	mText->setFontSize(24.0f);
	mText->showText();
}

void MessageWindow::inputActor()
{
}

void MessageWindow::updateActor()
{
	std::wstring message = L"x:" + std::to_wstring(mCamera->getPosition().x) +
		L" y:" + std::to_wstring(mCamera->getPosition().y) +
		L" z:" + std::to_wstring(mCamera->getPosition().z);

	mMessage = message;
	mText->setText(mMessage);
}

void MessageWindow::setCamera(const std::shared_ptr<Camera>& camera)
{
	mCamera = camera;
}
