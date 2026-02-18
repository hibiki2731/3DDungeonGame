#include "Player.h"
#include "CameraComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "Math.h"
#include "timer.h"
#include "CharacterComponent.h"
#include "DamageText.h"
#include "EnemyComponent.h"
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

		float moveLength = deltaTime * mMoveSpeed;
		float rotLength = deltaTime * mRotSpeed;

		//位置の更新
		if (fabsf(diffPos.x) > moveLength || fabsf(diffPos.y) >moveLength || fabsf(diffPos.z) >moveLength) {
			mPosition = mPosition + Math::normalize(diffPos) * moveLength;
		}
		else {
			mPosition = mTargetPos;
		}
		////回転の更新
		if (fabsf(diffRot.x) > rotLength || fabsf(diffRot.y) > rotLength || fabsf(diffRot.z) > rotLength) {

			mRotation = mRotation + Math::normalize(diffRot) * rotLength;
		}
		else {
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

			//インデックス位置の更新
			std::vector<int> preIndexPos = mCharacter->getIndexPos();
			//インデックス座標の更新
			preIndexPos[0] = static_cast<int>(std::round(mPosition.x / MAPTIPSIZE));
			preIndexPos[1] = static_cast<int>(std::round(mPosition.z / MAPTIPSIZE));
			mCharacter->setIndexPos(preIndexPos);

			//マップ上のオブジェクトデータ更新
			mCharacter->getMapManager()->setObjectDataAt(mCharacter->getIndexPosInt(), ObjectType::EMPTY);
			mCharacter->getMapManager()->setObjectDataAt(mCharacter->getIndexPosInt(), ObjectType::PLAYER);
		}
	}

	if (mActionTimer > 0.0f) mActionTimer -= deltaTime;
}

int Player::getDirection()
{
	return mCharacter->getDirection();
}

void Player::attack()
{
	if (mActionTimer > 0.0f) return;
	if (isMoving) return;

	//前方のエネミーを取得
	std::shared_ptr<EnemyComponent> target = nullptr;
	switch (mCharacter->getDirection()) {
	case Direction::UP:
		target = getGame()->getEnemyFromIndexPos(std::vector<int>{mCharacter->getIndexPos()[0], mCharacter->getIndexPos()[1] + 1});
		break;
	case Direction::DOWN:
		target = getGame()->getEnemyFromIndexPos(std::vector<int>{mCharacter->getIndexPos()[0], mCharacter->getIndexPos()[1] - 1});
		break;
	case Direction::RIGHT:
		target = getGame()->getEnemyFromIndexPos(std::vector<int>{mCharacter->getIndexPos()[0] + 1, mCharacter->getIndexPos()[1]});
		break;
	case Direction::LEFT:
		target = getGame()->getEnemyFromIndexPos(std::vector<int>{mCharacter->getIndexPos()[0] - 1, mCharacter->getIndexPos()[1]});
		break;
	}

	if (target == nullptr) { 
		return; 
	}

	//ダメージの計算
	int damage = max(mCharacter->getPower() - target->getDefense(), 0);
	target->giveDamage(damage); //ダメージを与える

	//ダメージエフェクト
	target->startFlash(); //敵を点滅させる
	calcDamageText(target->getPosition(), damage);

	mActionTimer = 1.0f;
}

void Player::calcDamageText(const XMFLOAT3& targetPos, int val)
{
	XMFLOAT3 textPos = targetPos;
	textPos.y += 0.5f; //少しだけ上に

	XMFLOAT3 front = Math::rotateY(XMFLOAT3(0.0f, 0.0f, 1.0f), mRotation.y);//前方ベクトル
	XMFLOAT3 right = Math::rotateY(front, -XM_PIDIV2);//右ベクトル
	textPos = textPos - front;	//敵オブジェクトより手前に

	int digit = 0;			//桁数
	int value = val;		//表示したい数値
	std::vector<int> num;	//各桁の値

	//桁数と各桁の値を取得
	while (value > 0) {
		digit++;
		num.push_back(value % 10);
		value = value / 10;
	}

	float DTHalfSize = mGame->getDamageTextManager()->getSize() * 0.5f;
	//数値が画面中心に来るよう調整
	textPos = textPos + (right * DTHalfSize * 0.5 * (digit - 1));
	//桁ごとの表示位置の調整
	for (int i = 0; i < digit; i++) {
		mGame->getDamageTextManager()->createDamageText(textPos, num[i]);
		textPos = textPos - right * DTHalfSize;
	}
}
