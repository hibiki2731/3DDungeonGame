#include "RockObject.h"
#include "MeshComponent.h"

void RockWall_side::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create(ObjectName::ROCK_WALL_SIDE);
}

void RockWall::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create(ObjectName::ROCK_WALL);
}

void RockFloor::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create(ObjectName::ROCK_FLOOR);
}
