#pragma once
#include "Actor.h"
class PointLight :
    public Actor
{
public:
    PointLight() {};
    ~PointLight() {};

    void initActor() override;
    void updateActor() override;
    void inputActor() override;
};

