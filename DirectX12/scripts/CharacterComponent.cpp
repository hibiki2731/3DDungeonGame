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
	mIndexPos = std::vector<int>(2,0);

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

std::vector<int>& CharacterComponent::getIndexPos()
{
	return mIndexPos;
}

int CharacterComponent::getIndexPosInt()
{
	return mIndexPos[1] * mMapManager->getMapSize() + mIndexPos[0];
}

std::shared_ptr<MapManager> CharacterComponent::getMapManager()
{
	return mMapManager;
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

void CharacterComponent::setIndexPos(const std::vector<int>& indexPos)
{
	if (indexPos.size() != 2) assert(false);
	mIndexPos = indexPos;
}

void CharacterComponent::setIndexPosInt(int indexPos)
{
	mIndexPos[0] = indexPos % mMapManager->getMapSize();
	mIndexPos[1] = indexPos / mMapManager->getMapSize();
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

