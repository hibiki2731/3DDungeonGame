#pragma once
#include <DirectXMath.h>
#include "Component.h"
#include "Buffer.h"
using namespace DirectX;

class PointLightComponent : public Component
{
public:
	PointLightComponent() {};
	~PointLightComponent() {};

	void initComponent() override;
	void inputComponent() override;
	void updateComponent() override;
	void endProccess() override;

	//ゲッター
	XMFLOAT4 getPosition();
	XMFLOAT4 getColor();
	bool getActive();
	float getRange();
	float getIntensity();
	//セッター
	void setColor(const XMFLOAT4 color);
	void setActive(const bool state);
	void setIntensity(const float intensity);
	void setRange(const float range);

private:
	bool isActive;
	float mRange;
	float mIntensity;
	XMFLOAT4 mColor;
	XMFLOAT4 mPosition;
};

