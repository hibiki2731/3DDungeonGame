#pragma once
#include "Actor.h"

class Player : public Actor {
public:
	//コンストラクタは使わない
	Player();
	~Player();
	void inputActor() override;
	void updateActor() override;
	void initActor() override;

private:
};
