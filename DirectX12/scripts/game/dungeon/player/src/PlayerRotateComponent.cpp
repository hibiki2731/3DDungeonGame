#include "PlayerRotateComponent.h"
#include "Actor.h"
#include "CharacterComponent.h"
#include "DungeonScene.h"
#include "MyMath.h"
#include "timer.h"

PlayerRotateComponent::PlayerRotateComponent(DungeonScene& scene, Actor& owner, CharacterComponent& character, float rotSpeed)
	: Component(owner),
	mScene(scene),
	mCharacter(character),
	mRotSpeed(rotSpeed),
	mTargetRot(owner.getRotation())
{
}

void PlayerRotateComponent::startRotate(Direction direction)
{
	switch (direction) {
	case Direction::RIGHT:
		mTargetRot = mOwner.getRotation() + XMFLOAT3(0, XM_PIDIV2, 0);
		mCharacter.turnRight();	//向きの更新
		break;
	case Direction::LEFT:
		mTargetRot = mOwner.getRotation() - XMFLOAT3(0, XM_PIDIV2, 0);
		mCharacter.turnLeft();	//向きの更新
		break;
	default:
		return;
	}

	mScene.updateMiniMapDirection();	//ミニマップのアイコンの向きを更新
}

bool PlayerRotateComponent::updateRotate()
{
	const XMFLOAT3 rotation = mOwner.getRotation();
	const XMFLOAT3 diffRot = mTargetRot - rotation;
	const float rotLength = deltaTime * mRotSpeed;

	//回転の更新 diffRot(現在の回転角と目的の回転角の差分)よりrotLength(回転量)が大きくなるまで現在の回転にrotLengthを加算
	if (fabsf(diffRot.x) > rotLength || fabsf(diffRot.y) > rotLength || fabsf(diffRot.z) > rotLength) {
		mOwner.setRotation(rotation + Math::normalize(diffRot) * rotLength);
		return false;
	}

	//終了時は目的の回転角にピッタリ合わせる
	mOwner.setRotation(mTargetRot);
	return true;
}
