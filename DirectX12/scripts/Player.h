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

	int getDirection();

private:
	void attack();
	void calcDamageText(const XMFLOAT3& targetPos, int val);

	float mMoveSpeed;
	float mRotSpeed;

	//移動処理用
	XMFLOAT3 mTargetPos;
	XMFLOAT3 mTargetRot;
	float isMoving;

	//カウンター
	float mActionTimer = 0.0f;

	std::shared_ptr<CameraComponent> mCamera;
	std::shared_ptr<CharacterComponent> mCharacter;
};

