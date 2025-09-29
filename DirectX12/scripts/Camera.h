#pragma once
#include "Actor.h"

class CameraComponent;

class Camera : public Actor
{
public:
	Camera() {};
	~Camera() {};

	void initActor() override;

	void inputActor() override;
	void updateActor() override;

private:
	float mMoveSpeed;
	float mRotSpeed;

	std::shared_ptr<CameraComponent> mCamera;
};

