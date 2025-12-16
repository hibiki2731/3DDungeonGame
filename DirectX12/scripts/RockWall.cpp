#include "rockWall.h"
#include "MeshComponent.h"

void RockWall_side::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create("assets\\rock_side\\wall_side.txt");
}

void RockWall::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	//mesh->create("assets\\rockObj\\rockWall.txt");
	mesh->create("assets\\rockObj\\rockWall.txt");
}

void RockFloor::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create("assets\\rockObj\\rockFloor.txt");
}
