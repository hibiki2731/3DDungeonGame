#pragma once
#include "Actor.h"

class Anime2DComponent;

class UI : public Actor
{
public:
	UI() {};
	~UI() {};

	void initActor() override;
	void updateActor() override;

private:
	std::shared_ptr<Anime2DComponent> mAnime;
	int mTextureIndex;
	int counter;
};

