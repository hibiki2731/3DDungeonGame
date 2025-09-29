#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Math.h"
#include <windows.h>

void CameraComponent::initComponent()
{
	mFront = { 0, 0, 1.0f };  mUp = { 0, 1, 0 };
	mFocus = mOwner->getPosition() + mFront;
	mRot = 0;
	isActive = false;
}

void CameraComponent::inputComponent()
{
}

void CameraComponent::updateComponent()
{
	if (isActive) {
		mFront = { sinf(mRot), 0, cosf(mRot) };
		mFocus = mOwner->getPosition() + mFront;
		XMFLOAT3 eye = mOwner->getPosition();


		XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&mFocus), XMLoadFloat3(&mUp));
		//プロジェクションマトリックス
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, mOwner->getGame()->getGraphic()->getAspect(), 1.0f, 50.0f);
		XMMATRIX viewProj = view * proj;
		mOwner->getGame()->getGraphic()->updateViewProj(viewProj);
	}

}

void CameraComponent::setActive(bool state)
{
	isActive = state;
}

const float& CameraComponent::getRot()
{
	return mRot;
}

const XMFLOAT3& CameraComponent::getFront()
{
	return mFront;
}

void CameraComponent::setRot(float rot)
{
	mRot = rot;
}
