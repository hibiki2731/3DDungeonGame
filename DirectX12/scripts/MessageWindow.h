#pragma once
#include "Actor.h"
#include <string>

class Camera;
class TextComponent;

class MessageWindow : public Actor
{
public:
	MessageWindow() {};
	~MessageWindow() {};

	void initActor() override;

	void inputActor() override;
	void updateActor() override;

	void setCamera(const std::shared_ptr<Camera>& camera);

private:

	std::wstring mMessage;
	std::shared_ptr<Camera> mCamera;
	std::shared_ptr<TextComponent> mText;
};

