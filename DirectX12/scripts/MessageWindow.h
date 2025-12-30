#pragma once
#include "Actor.h"
#include <string>

class TextComponent;

class MessageWindow : public Actor
{
public:
	MessageWindow() {};
	~MessageWindow() {};

	void initActor() override;

	void inputActor() override;
	void updateActor() override;

	void setActor(const std::shared_ptr<Actor>& actor);

private:

	std::wstring mMessage;
	std::shared_ptr<Actor> mActor;
	std::shared_ptr<TextComponent> mText;
};

