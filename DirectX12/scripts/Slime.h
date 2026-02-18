#pragma once
#include "Actor.h"

class EnemyComponent;

class Slime : public Actor
{
public:
	void initActor() override;
	void updateActor() override;
	void inputActor() override;

private:
	std::shared_ptr<class EnemyComponent> mEnemy;
};

