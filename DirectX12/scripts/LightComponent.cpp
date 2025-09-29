#include <windows.h>
#include "Math.h"
#include "timer.h"
#include "LightComponent.h"
#include "Actor.h"
#include "Game.h"

void LightComponent::initComponent()
{
	isActive = false;
}

void LightComponent::inputComponent()
{
}

void LightComponent::updateComponent()
{
	if (isActive) {
		XMFLOAT4 lightPos;
		lightPos.x = mOwner->getPosition().x;
		lightPos.y = mOwner->getPosition().y;
		lightPos.z = mOwner->getPosition().z;
		lightPos.w = 0;

		//ライトベクトル
		XMVECTOR v = XMLoadFloat4(&lightPos);
		XMVECTOR n = XMVector3Normalize(v);
		XMStoreFloat4(&lightPos, n);
		mOwner->getGame()->getGraphic()->updateLightPos(lightPos);
	}
}

void LightComponent::setActive(bool state)
{
	isActive = state;
}
