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
#include "MapManager.h"
#include "Graphic.h"

void Player::initActor()
{
	setYPos(0.5f);
	mTargetPos = mPosition;
	mTargetRot = mRotation;
	mMoveSpeed = 5.0f;
	mRotSpeed = 4.5f;
	mCamera = createComponent<CameraComponent>(shared_from_this());
	mCamera->setActive(true);
	isMoving = false;
	isRotating = false;
	mActionTimer = 0.0f;

	auto spotLight = createComponent<SpotLightComponent>(shared_from_this());
	spotLight->setActive(true);
	spotLight->setColor(XMFLOAT4(1.0f, 0.9f, 0.8f, 1.0f));
	spotLight->setIntensity(30.0f);
	spotLight->setRange(50.0f);
	spotLight->setUAngle(XMConvertToRadians(10.0f));
	spotLight->setPAngle(XMConvertToRadians(40.0f));

	mCharacter = createComponent<CharacterComponent>(shared_from_this());
	mCharacter->setDirection(Direction::UP);
	mMapManager = mGame->getMapManager();
	mGame->setPlayer(dynamic_pointer_cast<Player>(shared_from_this()));

}

void Player::inputActor()
{

	if (GetAsyncKeyState('A')) {
		move(Direction::LEFT);
	}
	if (GetAsyncKeyState('D')) {
		move(Direction::RIGHT);
	}
	if (GetAsyncKeyState('W')) {
		move(Direction::UP);
	}
	if (GetAsyncKeyState('S')) {
		move(Direction::DOWN);
	}
	if (GetAsyncKeyState(VK_RIGHT)) {
		rotate(Direction::RIGHT);
	}
	if (GetAsyncKeyState(VK_LEFT)) {
		rotate(Direction::LEFT);
	}
	if (GetAsyncKeyState(VK_RETURN)) {
		if (mActionTimer <= 0) 	attack();
		if (mActionTimer <= 0) 	collect();
	}
	
}

void Player::updateActor()
{
	mActionTimer -= deltaTime;
	//移動処理
	if (isMoving) {
		//移動処理
		XMFLOAT3 diffPos = mTargetPos - mPosition;

		float moveLength = deltaTime * mMoveSpeed;

		//位置の更新
		if (fabsf(diffPos.x) > moveLength || fabsf(diffPos.y) > moveLength || fabsf(diffPos.z) > moveLength) {
			mPosition = mPosition + Math::normalize(diffPos) * moveLength;
		}
		else {
			mPosition = mTargetPos;

			//ターン経過
			mMapManager->moveToEnemyTurn();
			isMoving = false;
		}
	}
	//カメラ回転時の処理
	if (isRotating) {
		XMFLOAT3 diffRot = mTargetRot - mRotation;
		float rotLength = deltaTime * mRotSpeed;

		////回転の更新
		if (fabsf(diffRot.x) > rotLength || fabsf(diffRot.y) > rotLength || fabsf(diffRot.z) > rotLength) {

			mRotation = mRotation + Math::normalize(diffRot) * rotLength;
		}
		//終了時の処理
		else {
			mRotation = mTargetRot;
			isRotating = false;
		}
	}

	mState = State::Active;
}

int Player::getDirection()
{
	return mCharacter->getDirection();
}

void Player::getIndexPos(int(&pos)[2])
{
	pos[0] = mCharacter->getIndexPos()[0];
	pos[1] = mCharacter->getIndexPos()[1];
}

int Player::getHP()
{
	return mCharacter->getHP();
}

int Player::getPower()
{
	return mCharacter->getPower();
}

int Player::getDefense()
{
	return mCharacter->getDefense();
}

void Player::giveDamage(int damage)
{
	int playerHP = mCharacter->getHP();
	mCharacter->setHP(playerHP - damage);
}

void Player::attack()
{
	//敵ターン時は実行不可
	if (mMapManager->getTurnType() == TurnType::ENEMY) return;
	//移動、回転中は実行不可
	if (isMoving || isRotating) return;

	//前方のエネミーを取得
	std::shared_ptr<EnemyComponent> target = nullptr;
	switch (mCharacter->getDirection()) {
	case Direction::UP:
		target = getGame()->getEnemyFromIndexPos(mCharacter->getIndexPos()[0], mCharacter->getIndexPos()[1] + 1);
		break;
	case Direction::DOWN:
		target = getGame()->getEnemyFromIndexPos(mCharacter->getIndexPos()[0], mCharacter->getIndexPos()[1] - 1);
		break;
	case Direction::RIGHT:
		target = getGame()->getEnemyFromIndexPos(mCharacter->getIndexPos()[0] + 1, mCharacter->getIndexPos()[1]);
		break;
	case Direction::LEFT:
		target = getGame()->getEnemyFromIndexPos(mCharacter->getIndexPos()[0] - 1, mCharacter->getIndexPos()[1]);
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

	mActionTimer = ACTION_WAIT_TIME;
	//ターン経過
	mMapManager->moveToEnemyTurn();
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

void Player::move(Direction direction)
{
	//プレイヤーターン時のみ実行
	if (mMapManager->getTurnType() == TurnType::ENEMY) return;
	//移動、回転中は実行不可
	if (isMoving || isRotating) return;

	int targetIndexPos[2] = {mCharacter->getIndexPos()[0], mCharacter->getIndexPos()[1]};
	//進行する差分のインデックスを取得
	calcMoveDirectionToIndexPos(direction, targetIndexPos);

	//進先に障害物がある場合移動不可
	if (mMapManager->getMapDataAt(targetIndexPos[0], targetIndexPos[1]) == TileType::WALL ||
		mMapManager->getObjectDataAt(targetIndexPos[0], targetIndexPos[1]) != ObjectType::EMPTY) return;

	//移動前の座標を空に
	mMapManager->setObjectDataAt(mCharacter->getIndexPosInt(), ObjectType::EMPTY);
	//マップ上のオブジェクトデータ更新
	mMapManager->setObjectDataAt(targetIndexPos[0], targetIndexPos[1], ObjectType::PLAYER);

	mTargetPos = XMFLOAT3(static_cast<float>(targetIndexPos[0]) * MAPTIPSIZE, mPosition.y, static_cast<float>(targetIndexPos[1]) * MAPTIPSIZE);
	
	isMoving = true;

}	

void Player::rotate(Direction direction)
{
	//プレイヤーターン時のみ実行
	if (mMapManager->getTurnType() == TurnType::ENEMY) return;
	//移動、回転中は実行不可
	if (isMoving || isRotating) return;

	switch (direction) {
	case Direction::RIGHT:
		mTargetRot = mRotation - XMFLOAT3(0, XM_PIDIV2, 0);
		mCharacter->turnRight(); //向きの更新

		break;
	case Direction::LEFT:
		mTargetRot = mRotation + XMFLOAT3(0, XM_PIDIV2, 0);
		mCharacter->turnLeft(); //向きの更新
		break;
	}

	isRotating = true;
}

void Player::calcMoveDirectionToIndexPos(Direction moveDirection, int (&indexPos)[2])
{
	//動く方向と向いている方向から、最終的に動く向きをもとめる
	int index = moveDirection | mCharacter->getDirection(); 

	//インデックス座標の変位をもとめる
	switch (index) {
	case 0b0100:
	case 0b0001:
	case 0b1010:
		indexPos[1] += 1;
		break;
	case 0b0101:
	case 0b0010:
	case 0b1000:
		indexPos[1] -= 1;
		break;
	case 0b0110:
	case 0b1001:
		indexPos[0] += 1;
		break;
	case 0b1100:
	case 0b0011:
		indexPos[0] -= 1;
		break;
	}

}

void Player::collect()
{
	//プレイヤーターン時のみ実行
	if (mMapManager->getTurnType() == TurnType::ENEMY) return;
	//移動、回転中は実行不可
	if (isMoving || isRotating) return;

	int tileData = mMapManager->getMapDataAt(mCharacter->getIndexPosInt());

	//今いるマスが通常の床ならば何も行わない
	if (tileData <= 1) return;

	switch (tileData) {
	case TileType::GRASS:
		mGame->getItemManager()->addItem(Item::GRASS, 1);
	}

	//ターン経過
	mMapManager->moveToEnemyTurn();

	mActionTimer = ACTION_WAIT_TIME;
}