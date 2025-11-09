#include "PointLight.h"
#include "PointLightComponent.h"
#include "timer.h"
#include "Game.h"
#include <windows.h>

void PointLight::initActor()
{
	auto light = createComponent<PointLightComponent>(shared_from_this());
	light->setActive(true);
	light->setColor(XMFLOAT4(0.1f, 0.3f, 1.0f, 1.0f));
	light->setIntensity(3.0f);
	light->setRange(2.0f);
}

void PointLight::updateActor()
{
}

void PointLight::inputActor()
{
}
