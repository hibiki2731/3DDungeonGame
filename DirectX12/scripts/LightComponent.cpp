#include <windows.h>
#include "Math.h"
#include "timer.h"
#include "LightComponent.h"
#include "Actor.h"
#include "Game.h"

void LightComponent::initComponent()
{
	isActive = false;
	mOwner->getGame()->addLight(std::dynamic_pointer_cast<LightComponent>(shared_from_this()));
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
		lightPos.w = isActive;

		mPosition = lightPos;

	}
}

void LightComponent::endProccess()
{
	//Game‚©‚çƒ‰ƒCƒg‚ðíœ
	mOwner->getGame()->removeLight(std::dynamic_pointer_cast<LightComponent>(shared_from_this()));
}

XMFLOAT4 LightComponent::getPosition()
{
	return mPosition;
}

void LightComponent::setActive(bool state)
{
	isActive = state;
}
