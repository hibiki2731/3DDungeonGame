#include "Slime.h"
#include "MeshComponent.h"

void Slime::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create("assets\\slime.txt");
	setScale(XMFLOAT3(0.3f, 0.3f, 0.3f));
}

void Slime::updateActor()
{
}

void Slime::inputActor()
{
}
