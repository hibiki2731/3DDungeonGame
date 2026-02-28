#pragma once
#include "Actor.h"


class RockWall_side : public Actor
{
public:
	RockWall_side(Game* game, float x, float y);
};

class RockWall : public Actor {
public:
	RockWall(Game* game, float x, float y);
};

class RockFloor : public Actor {
public:
	RockFloor(Game* game, float x, float y);
};