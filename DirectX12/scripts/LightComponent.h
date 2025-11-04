#pragma once
#include <DirectXMath.h>
#include "Component.h"
#include "Buffer.h"
using namespace DirectX;

class LightComponent : public Component
{
public:
	LightComponent() {};
	~LightComponent() {};

	void initComponent() override;
	void inputComponent() override;
	void updateComponent() override;

	void setActive(bool state);

private:
	bool isActive;
	Light mLight;
};

