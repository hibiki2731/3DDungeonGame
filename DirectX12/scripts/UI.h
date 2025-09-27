#pragma once
#include "Actor.h"

class SpriteComponent;

class UI : public Actor
{
public:
	UI() {};
	~UI() {};

	void initActor() override;
	void updateActor() override;

private:
	std::shared_ptr<SpriteComponent> mSprite;
	int mTextureIndex;
	int counter;
};

