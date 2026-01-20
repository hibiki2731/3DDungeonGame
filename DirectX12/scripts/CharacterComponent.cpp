#include "CharacterComponent.h"
#include "Actor.h"
#include "Game.h"

void CharacterComponent::initComponent()
{
	mMaxHP = 100;
	mHP = mMaxHP;
	mPower = 20;
	mDefense = 5;
	isAlive = true;

	mDirection = 1;
	mIndexPos = XMFLOAT2(0.0f, 0.0f);

	mOwner->getGame()->addCharacter(std::dynamic_pointer_cast<CharacterComponent>(shared_from_this()));
	mMapManager = mOwner->getGame()->getMapManager();
}

void CharacterComponent::inputComponent()
{
}

void CharacterComponent::updateComponent()
{
}

void CharacterComponent::endProccess()
{
	mOwner->getGame()->removeCharacter(std::dynamic_pointer_cast<CharacterComponent>(shared_from_this()));
}

int CharacterComponent::getHP()
{
	return mHP;
}

int CharacterComponent::getPower()
{
	return mPower;
}

int CharacterComponent::getDefense()
{
	return mDefense;
}

int CharacterComponent::getDirection()
{
	return mDirection;
}

bool CharacterComponent::getAlive()
{
	return isAlive;
}

XMFLOAT2 CharacterComponent::getIndexPos()
{
	return mIndexPos;
}

int CharacterComponent::getIndexPosInt()
{
	return mIndexPos.y * mMapManager->getMapSize() + mIndexPos.x;
}

std::shared_ptr<MapManager> CharacterComponent::getMapManager()
{
	return mMapManager;
}

std::shared_ptr<CharacterComponent> CharacterComponent::getCharacterFromIndexPos(XMFLOAT2 indexPos)
{
	auto characters = mOwner->getGame()->getCharacters();
	for (auto& character : *characters) {
		XMFLOAT2 charIndexPos = character->getIndexPos();
		if (charIndexPos.x == indexPos.x && charIndexPos.y == indexPos.y) {
			return character;
		}
	}
	return nullptr;
}

std::shared_ptr<CharacterComponent> CharacterComponent::getCharacterFromIndexPos(int index)
{
	int mapSize = mMapManager->getMapSize();
	XMFLOAT2 indexPos;
	indexPos.x = index % mapSize;
	indexPos.y = index / mapSize;
	return getCharacterFromIndexPos(indexPos);
}

void CharacterComponent::setMaxHP(int maxHP)
{
	mMaxHP = maxHP;
}

void CharacterComponent::setHP(int hp)
{
	mHP = hp;
}

void CharacterComponent::setPower(int power)
{
	mPower = power;
}

void CharacterComponent::setDefense(int defense)
{
	mDefense = defense;
}

void CharacterComponent::setDirection(int direction)
{
	mDirection = direction;
}

void CharacterComponent::setIndexPos(XMFLOAT2 indexPos)
{
	mIndexPos = indexPos;
}

void CharacterComponent::setIndexPos(int index)
{
	int mapSize = mMapManager->getMapSize();
	mIndexPos.x = index % mapSize;
	mIndexPos.y = index / mapSize;
}

void CharacterComponent::giveDamage(int damage)
{
	mHP -= damage;
	if (mHP < 0) {
		mHP = 0;
		isAlive = false;
	}
}

void CharacterComponent::turnRight()
{
	mDirection = mDirection >> 1;
	if (mDirection < Direction::DOWN) {
		mDirection = Direction::LEFT;
	}
}

void CharacterComponent::turnLeft()
{
	mDirection = mDirection << 1;
	if (mDirection > Direction::LEFT) {
		mDirection = Direction::DOWN;
	}
}

