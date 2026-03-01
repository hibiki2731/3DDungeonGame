#include "RockObject.h"
#include "MeshComponent.h"

RockWall_side::RockWall_side(Game* game, float x, float y) : Actor(game)
{
	mPosition = XMFLOAT3(x, 0, y);
	auto mesh = std::make_unique<MeshComponent>(this);
	mesh->create(MeshName::ROCK_WALL_SIDE);
	addComponent(std::move(mesh));
}

RockWall::RockWall(Game* game, float x, float y) : Actor(game)
{
	mPosition = XMFLOAT3(x, 0, y);
	auto mesh = std::make_unique<MeshComponent>(this);
	mesh->create(MeshName::ROCK_WALL);
	addComponent(std::move(mesh));
}

RockFloor::RockFloor(Game* game, float x, float y) : Actor(game)
{
	mPosition = XMFLOAT3(x, 0, y);
	auto mesh = std::make_unique<MeshComponent>(this);
	mesh->create(MeshName::ROCK_FLOOR);
	addComponent(std::move(mesh));
}