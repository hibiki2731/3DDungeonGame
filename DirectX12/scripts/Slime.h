#pragma once
#include "Actor.h"

class CharacterComponent;

class Slime : public Actor
{
public:
	void initActor() override;
	void updateActor() override;
	void inputActor() override;

private:
	std::shared_ptr<class CharacterComponent> mCharacter;
};

