#pragma once
#include "Actor.h"
#include <string>

class TextComponent;
class Player;

class MessageWindow : public Actor
{
public:
	MessageWindow() {};
	~MessageWindow() {};

	void initActor() override;

	void inputActor() override;
	void updateActor() override;

	void setTarget(const std::shared_ptr<Actor>& actor);
	void setPlayer(const std::shared_ptr<Player>& player);

private:

	std::wstring mMessage;
	std::shared_ptr<Actor> mTarget;
	std::shared_ptr<Player> mPlayer;
	std::shared_ptr<TextComponent> mText;
};

