#include "PointLight.h"
#include "LightComponent.h"
#include "timer.h"
#include <windows.h>

void PointLight::initActor()
{
	mPosition = {0, 1.0f, -5.0f};
	auto light = createComponent<LightComponent>(shared_from_this());
	light->setActive(true);
}

void PointLight::updateActor()
{
}

void PointLight::inputActor()
{
}
