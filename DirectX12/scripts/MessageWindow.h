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

	void setTarget(Actor* actor);
	void setPlayer(Player* player);

private:

	std::wstring mMessage;
	Actor* mTarget;
	Player* mPlayer;
	std::shared_ptr<TextComponent> mText;
};

