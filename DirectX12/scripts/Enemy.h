#pragma once
#include "Actor.h"

class EnemyComponent;

class Enemy : public Actor
{
public:
	Enemy(Game* game, float x, float y);

	void updateActor() override;
	void inputActor() override;

private:
	class EnemyComponent* mEnemy;
};

