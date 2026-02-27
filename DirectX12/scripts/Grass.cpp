#include "Grass.h"
#include "MeshComponent.h"

void Grass::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create(ObjectName::GRASS);

}

void Grass::updateActor() {
}

void Grass::inputActor(){}
