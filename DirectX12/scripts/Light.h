#pragma once
#include "Actor.h"
class Light :
    public Actor
{
public:
    Light() {};
    ~Light() {};

    void initActor() override;
    void updateActor() override;
    void inputActor() override;
};

