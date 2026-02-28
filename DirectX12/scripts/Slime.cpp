#include "Slime.h"
#include "MeshComponent.h"
#include "Game.h"
#include "EnemyComponent.h"
#include "MapManager.h"
#include "Player.h"

Slime::Slime(Game* game, float x, float y) : Actor(game)
{
	mPosition = XMFLOAT3(x, 0, y);
	auto mesh = std::make_unique<MeshComponent>(this);
	mesh->create(ObjectName::SLIME);
	setScale(XMFLOAT3(0.3f, 0.3f, 0.3f));

	auto enemy = std::make_unique<EnemyComponent>(this);
	mEnemy = enemy.get();
	mEnemy->setMesh(mesh.get());
	mEnemy->setEnemyTtype(ObjectType::SLIME);
	mEnemy->setDirection(Direction::UP); //ãŒü‚«
	mEnemy->setDefense(3);
	mEnemy->setPower(5);
	mEnemy->setMaxHP(10);
	mEnemy->setEnemyState(EnemyState::RANDOM);

	addComponent(std::move(mesh));
	addComponent(std::move(enemy));
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