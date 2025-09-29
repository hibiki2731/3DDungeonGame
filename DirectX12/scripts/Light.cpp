#include "Light.h"
#include "LightComponent.h"
#include "timer.h"
#include <windows.h>

void Light::initActor()
{
	mPosition = {0, 1.0f, -5.0f};
	auto light = createComponent<LightComponent>(shared_from_this());
	light->setActive(true);
}

void Light::updateActor()
{
}

void Light::inputActor()
{
}
