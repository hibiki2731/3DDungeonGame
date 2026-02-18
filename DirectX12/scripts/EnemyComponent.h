#pragma once
#include "CharacterComponent.h"
#include "MeshComponent.h"

class EnemyComponent : public CharacterComponent
{
public:
	void initComponent() override;
	void inputComponent() override;
	void updateComponent() override;
	void endProccess() override;

	void setMesh(const std::shared_ptr<MeshComponent>& mesh);

	void startFlash();

	//getter
	XMFLOAT3 getPosition();

private:
	std::shared_ptr<MeshComponent> mMesh;
	float mFlashTimer;
	float mFlashDuration;

	void updateFlash();
};

