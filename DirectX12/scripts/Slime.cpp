#include "Slime.h"
#include "MeshComponent.h"
#include "Game.h"
#include "CharacterComponent.h"
#include "MapManager.h"

void Slime::initActor()
{
	auto mesh = createComponent<MeshComponent>(shared_from_this());
	mesh->create("assets\\slime.txt");
	setScale(XMFLOAT3(0.3f, 0.3f, 0.3f));

	mCharacter = createComponent<CharacterComponent>(shared_from_this());

}

void Slime::updateActor()
{
	if (!mCharacter->getAlive()) {
		mGame->removeActor(shared_from_this());
	}

	mCharacter->setIndexPos(XMFLOAT2(getPosition().x / MAPTIPSIZE, getPosition().z / MAPTIPSIZE));
}

void Slime::inputActor()
{
}