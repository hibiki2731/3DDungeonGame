#include "Slime.h"
#include "MeshComponent.h"
#include "Game.h"
#include "EnemyComponent.h"
#include "MapManager.h"

void Slime::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create("assets\\slime.txt");
	setScale(XMFLOAT3(0.3f, 0.3f, 0.3f));

	mEnemy = createComponent<EnemyComponent>(shared_from_this());
	mEnemy->setMesh(mesh);
	mEnemy->setDirection(Direction::UP); //ãŒü‚«

}

void Slime::updateActor()
{
}

void Slime::inputActor()
{
}