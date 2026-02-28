#pragma once
#include "Actor.h"

class EnemyComponent;

class Slime : public Actor
{
public:
	Slime(Game* game, float x, float y);

	void updateActor() override;
	void inputActor() override;

private:
	class EnemyComponent* mEnemy;
	int mPlayerIndexPos[2];
};

