#pragma once
#include "Actor.h"

class MessageWindow : public Actor
{
public:
	MessageWindow() {};
	~MessageWindow() {};

	void initActor() override;

	void inputActor() override;
	void updateActor() override;
};

