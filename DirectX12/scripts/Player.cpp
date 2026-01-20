#include "Player.h"
#include "CameraComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "Math.h"
#include "timer.h"
#include "CharacterComponent.h"
#include <windows.h>
#include <algorithm>
#include <cmath>
#include "Game.h"
#include "Graphic.h"

void Player::initActor()
{
	mPosition = { 0, 0.5f, 0.0f };
	mTargetPos = mPosition;
	mTargetRot = mRotation;
	isMoving = false;
	mMoveSpeed = 5.0f;
	mRotSpeed = 4.5f;
	mCamera = createComponent<CameraComponent>(shared_from_this());
	mCamera->setActive(true);

	auto spotLight = createComponent<SpotLightComponent>(shared_from_this());
	spotLight->setActive(true);
	spotLight->setColor(XMFLOAT4(1.0f, 0.9f, 0.8f, 1.0f));
	spotLight->setIntensity(30.0f);
	spotLight->setRange(50.0f);
	spotLight->setUAngle(XMConvertToRadians(10.0f));
	spotLight->setPAngle(XMConvertToRadians(40.0f));

	mCharacter = createComponent<CharacterComponent>(shared_from_this());
	mCharacter->setDirection(Direction::UP);
	mCharacter->setIndexPos(XMFLOAT2(0.0f, 0.0f));

}

void Player::inputActor()
{

		if (GetAsyncKeyState('A')) {
			if (isMoving) return;
			mTargetPos = mPosition + Math::rotateY(normalZ, mRotation.y + XM_PIDIV2) * MAPTIPSIZE;
			isMoving = true;
		}
		if (GetAsyncKeyState('D')) {
			if (isMoving) return;
			mTargetPos = mPosition + Math::rotateY(normalZ, mRotation.y - XM_PIDIV2) * MAPTIPSIZE;
			isMoving = true;
		}
		if (GetAsyncKeyState('W')) {
			if (isMoving) return;
			mTargetPos = mPosition + Math::rotateY(normalZ, mRotation.y) * MAPTIPSIZE;
			isMoving = true;
		}
		if (GetAsyncKeyState('S')) {
			if (isMoving) return;
			mTargetPos = mPosition - Math::rotateY(normalZ, mRotation.y) * MAPTIPSIZE;
			isMoving = true;
		}
		if (GetAsyncKeyState(VK_UP)) {
			if (isMoving) return;
			mRotation = mRotation - XMFLOAT3(mRotSpeed * deltaTime, 0, 0);
			isMoving = true;
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			if (isMoving) return;
			mRotation = mRotation + XMFLOAT3(mRotSpeed * deltaTime, 0, 0);
			isMoving = true;
		}
		if (GetAsyncKeyState(VK_RIGHT)) {
			if (isMoving) return;
			mTargetRot = mRotation - XMFLOAT3(0, XM_PIDIV2, 0);
			mCharacter->turnRight();
			isMoving = true;
		}
		if (GetAsyncKeyState(VK_LEFT)) {
			if (isMoving) return;
			mTargetRot = mRotation + XMFLOAT3(0, XM_PIDIV2, 0);
			mCharacter->turnLeft();
			isMoving = true;
		}
		if (GetAsyncKeyState(VK_RETURN)) {
			if (isMoving) return;
			attack();

		}
	
}

void Player::updateActor()
{
	if (isMoving) {
		//移動処理
		XMFLOAT3 diffPos = mTargetPos - mPosition;
		XMFLOAT3 diffRot = mTargetRot - mRotation;

		//閾値
		float posMoveThreshold_a = 0.3f;
		float posMoveThreshold_b = 0.03f;
		float rotMoveThreshold_a = 0.3f;
		float rotMoveThreshold_b = 0.025f;

		//位置の更新
		if (fabsf(diffPos.x) > posMoveThreshold_a || fabsf(diffPos.y) > posMoveThreshold_a || fabsf(diffPos.z) > posMoveThreshold_a) {
			XMFLOAT3 delta = diffPos * deltaTime * mMoveSpeed;
			mPosition = mPosition + delta;
		}
		else if(fabsf(diffPos.x) > posMoveThreshold_b || fabsf(diffPos.y) > posMoveThreshold_b || fabsf(diffPos.z) > posMoveThreshold_b){
			mPosition = mPosition + Math::normalize(diffPos) * posMoveThreshold_b;
		}
		else {
			mPosition = mTargetPos;
		}
		////回転の更新
		if (fabsf(diffRot.x) > rotMoveThreshold_a || fabsf(diffRot.y) > rotMoveThreshold_a || fabsf(diffRot.z) > rotMoveThreshold_a) {

			mRotation = mRotation + diffRot * deltaTime * mRotSpeed;
		}
		else if (fabsf(diffRot.x) > rotMoveThreshold_b || fabsf(diffRot.y) > rotMoveThreshold_b || fabsf(diffRot.z) > rotMoveThreshold_b) {
			mRotation = mRotation + Math::normalize(diffRot) * rotMoveThreshold_b;
		}
		else{
			mRotation = mTargetRot;
		}
		//移動終了判定
		if (mPosition.x == mTargetPos.x &&
			mPosition.y == mTargetPos.y &&
			mPosition.z == mTargetPos.z &&
			mRotation.x == mTargetRot.x &&
			mRotation.y == mTargetRot.y &&
			mRotation.z == mTargetRot.z) {

			isMoving = false;

			//オブジェクトデータの更新
			mCharacter->getMapManager()->setObjectDataAt(mCharacter->getIndexPosInt(), ObjectType::EMPTY);
			mCharacter->setIndexPos(mPosition.x / MAPTIPSIZE + (mCharacter->getMapManager()->getMapSize() * (mPosition.z / MAPTIPSIZE)));
			mCharacter->getMapManager()->setObjectDataAt(mCharacter->getIndexPosInt(), ObjectType::PLAYER);
		}
	}
}

void Player::attack()
{
	std::shared_ptr<CharacterComponent> target = nullptr;
	switch (mCharacter->getDirection()) {
	case Direction::UP:
		target = mCharacter->getCharacterFromIndexPos(XMFLOAT2(mCharacter->getIndexPos().x, mCharacter->getIndexPos().y + 1.0f));
		break;
	case Direction::DOWN:
		target = mCharacter->getCharacterFromIndexPos(XMFLOAT2(mCharacter->getIndexPos().x, mCharacter->getIndexPos().y - 1.0f));
		break;
	case Direction::RIGHT:
		target = mCharacter->getCharacterFromIndexPos(XMFLOAT2(mCharacter->getIndexPos().x + 1.0f, mCharacter->getIndexPos().y));
		break;
	case Direction::LEFT:
		target = mCharacter->getCharacterFromIndexPos(XMFLOAT2(mCharacter->getIndexPos().x - 1.0f, mCharacter->getIndexPos().y));
		break;
	}

	if (target == nullptr) return;

	int damage = max(mCharacter->getPower() - target->getDefense(), 0);
	target->giveDamage(damage);
}
