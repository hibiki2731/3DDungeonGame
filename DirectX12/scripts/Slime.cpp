#include "Slime.h"
#include "MeshComponent.h"
#include "Game.h"
#include "EnemyComponent.h"
#include "MapManager.h"
#include "Player.h"

void Slime::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create(ObjectName::SLIME);
	setScale(XMFLOAT3(0.3f, 0.3f, 0.3f));

	mEnemy = createComponent<EnemyComponent>(shared_from_this());
	mEnemy->setMesh(mesh);
	mEnemy->setEnemyTtype(ObjectType::SLIME);
	mEnemy->setDirection(Direction::UP); //ãŒü‚«
	mEnemy->setDefense(3);
	mEnemy->setPower(5);
	mEnemy->setMaxHP(10);
	mEnemy->setEnemyState(EnemyState::RANDOM);

}

void Slime::updateActor()
{
	if (mEnemy->getDist() <= 2) {
		mEnemy->setEnemyState(EnemyState::ASTAR);
	}
	else mEnemy->setEnemyState(EnemyState::RANDOM);
}

void Slime::inputActor()
{
}