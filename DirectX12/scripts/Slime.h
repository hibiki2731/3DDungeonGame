#pragma once
#include "Actor.h"

class Slime : public Actor
{
public:
	void initActor() override;
	void updateActor() override;
	void inputActor() override;
};

