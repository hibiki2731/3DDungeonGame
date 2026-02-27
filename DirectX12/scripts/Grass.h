#pragma once
#include "Actor.h"

class Grass : public Actor
{
public:
	void initActor() override;
	void updateActor() override;
	void inputActor() override;

private:
};

