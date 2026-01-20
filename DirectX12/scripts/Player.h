#pragma once
#include "Actor.h"
#include <vector>

class CameraComponent;
class CharacterComponent;

class Player : public Actor
{
public:
	Player() {};
	~Player() {};

	void initActor() override;

	void inputActor() override;
	void updateActor() override;

private:
	void attack();

	float mMoveSpeed;
	float mRotSpeed;

	//ˆÚ“®ˆ——p
	XMFLOAT3 mTargetPos;
	XMFLOAT3 mTargetRot;
	float isMoving;

	std::shared_ptr<CameraComponent> mCamera;
	std::shared_ptr<CharacterComponent> mCharacter;
};

