#pragma once
#include "Actor.h"


class RockWall_side :
    public Actor
{
public:
    void initActor() override;
};

class RockWall : public Actor {
public:
    void initActor() override;
};

class RockFloor : public Actor {
public:
    void initActor() override;
};