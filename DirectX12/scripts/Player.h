#pragma once
#include "Math.h"
#include "timer.h"
#include "Actor.h"
#include "CameraComponent.h"

class Player : public Actor {
public:
	//コンストラクタは使わない
	Player();
	~Player();
	void inputActor() override;
	void updateActor() override;
	void initActor() override;

private:
	std::shared_ptr<CameraComponent> mCamera;
	float mMoveSpeed;
	float mRotSpeed;
};
