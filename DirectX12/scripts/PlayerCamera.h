#pragma once
#include "Actor.h"

class CameraComponent;

class PlayerCamera : public Actor
{
public:
	PlayerCamera() {};
	~PlayerCamera() {};

	void initActor() override;

	void inputActor() override;
	void updateActor() override;

private:
	float mMoveSpeed;
	float mRotSpeed;

	//ˆÚ“®ˆ——p
	XMFLOAT3 mTargetPos;
	XMFLOAT3 mTargetRot;
	float isMoving;

	std::shared_ptr<CameraComponent> mCamera;
};

