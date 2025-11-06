#include "PointLight.h"
#include "LightComponent.h"
#include "timer.h"
#include "Game.h"
#include <windows.h>

void PointLight::initActor()
{
	auto light = createComponent<LightComponent>(shared_from_this());
	light->setActive(true);
}

void PointLight::updateActor()
{
}

void PointLight::inputActor()
{
}
