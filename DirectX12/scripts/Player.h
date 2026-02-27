#pragma once
#include "Actor.h"
#include "Definition.h"
#include <vector>

class CameraComponent;
class CharacterComponent;
class MapManager;

class Player : public Actor
{
public:
	Player() {};
	~Player() {};

	void initActor() override;

	void inputActor() override;
	void updateActor() override;

	int getDirection();
	void getIndexPos(int(&pos)[2]);
	int getHP();
	int getPower();
	int getDefense();

	//プレイヤーにダメージを与える
	void giveDamage(int damage);

private:
	void attack();
	void calcDamageText(const XMFLOAT3& targetPos, int val);
	void move(Direction direction);
	void rotate(Direction direction);
	void calcMoveDirectionToIndexPos(Direction moveDirection, int (& indexPos)[2]);

	void collect();

	float mMoveSpeed;
	float mRotSpeed;

	//移動処理用
	XMFLOAT3 mTargetPos;
	XMFLOAT3 mTargetRot;
	bool isMoving;
	bool isRotating;

	//カウンター
	float mActionTimer;
	const float ACTION_WAIT_TIME = 0.5f;

	std::shared_ptr<CameraComponent> mCamera;
	std::shared_ptr<CharacterComponent> mCharacter;
	std::shared_ptr<MapManager> mMapManager;
};

