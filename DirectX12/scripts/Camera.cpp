#include "Camera.h"
#include "CameraComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "Math.h"
#include "timer.h"
#include "TextComponent.h"
#include <windows.h>
#include "Game.h"
#include "Graphic.h"

void Camera::initActor()
{
	mPosition = { 0, 1.0f, 0.0f };
	mMoveSpeed = 1.0f;
	mRotSpeed = 1.0f;
	mCamera = createComponent<CameraComponent>(shared_from_this());
	mCamera->setActive(true);

	auto spotLight = createComponent<SpotLightComponent>(shared_from_this());
	spotLight->setActive(true);
	spotLight->setColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	spotLight->setIntensity(2.0f);
	spotLight->setRange(10.0f);
	spotLight->setUAngle(XMConvertToRadians(20.0f));
	spotLight->setPAngle(XMConvertToRadians(40.0f));

	auto text = createComponent<TextComponent>(shared_from_this());
	D2D_SIZE_F rtSize = mGame->getGraphic()->getD2DRenderTarget()->GetSize();
	text->setBaseLine(rtSize.width / 2.0f, rtSize.height / 2.0f);
	text->setFontSize(64);
	text->setText(L"abcdefghijjj\njjjjklm\nnopqrstuvwxyz!?\n‚ ‚¢‚¤‚¦‚¨‚©‚«‚­‚¯‚±");
	text->showText();

}

void Camera::inputActor()
{


	if (GetAsyncKeyState('A')) {
		mPosition = mPosition + rotateY(normalZ, mRotation.y + PI / 2.0f) * mMoveSpeed * deltaTime;
	}
	if (GetAsyncKeyState('D')) {
		mPosition = mPosition + rotateY(normalZ,  mRotation.y - PI/2.0f) * mMoveSpeed * deltaTime;
	}
	if (GetAsyncKeyState('W')) {
		mPosition = mPosition + rotateY(normalZ, mRotation.y) * mMoveSpeed * deltaTime;
	}
	if (GetAsyncKeyState('S')) {
		mPosition = mPosition - rotateY(normalZ, mRotation.y) * mMoveSpeed * deltaTime;
	}
	if (GetAsyncKeyState(VK_UP)) {
		mRotation = mRotation - XMFLOAT3(mRotSpeed * deltaTime, 0, 0);
	}
	if (GetAsyncKeyState(VK_DOWN)) {
		mRotation = mRotation + XMFLOAT3(mRotSpeed * deltaTime, 0, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT)) {
		mRotation = mRotation - XMFLOAT3(0, mRotSpeed * deltaTime, 0);
	}
	if (GetAsyncKeyState(VK_LEFT)) {
		mRotation = mRotation + XMFLOAT3(0, mRotSpeed * deltaTime, 0);
	}
}

void Camera::updateActor()
{
}
