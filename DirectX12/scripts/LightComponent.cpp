#include <windows.h>
#include "Math.h"
#include "timer.h"
#include "LightComponent.h"
#include "Actor.h"
#include "Game.h"

void LightComponent::initComponent()
{
	isActive = false;
	mOwner->getGame()->addLight(mLight, 0);
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
		lightPos.w = 1.0f;

		//ライトベクトル
		mLight.position = lightPos;
		lightPos.x = 0.f;
		lightPos.y = 1.f;
		lightPos.z = 2.0f;
		lightPos.w = 1.0f;
		Light anotherLight;
		anotherLight.position = lightPos;
		mLight.isActive.x = ACTIVE;
		anotherLight.isActive.x = ACTIVE;

	}
}

void LightComponent::setActive(bool state)
{
	isActive = state;
}
