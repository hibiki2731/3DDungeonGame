#include "Actor.h"
#include "Component.h"
#include "Game.h"

void Actor::input()
{
	if (mState == Active) {
		for (auto& component : mComponents) {
			component->inputComponent();
		}
		inputActor();
	}
}

void Actor::update()
{
	if (mState == Active) {
		updateComponents();
		updateActor();
	}
}

void Actor::updateComponents()
{
	for (auto& component : mComponents) {
		component->updateComponent();
	}
}

void Actor::init(const std::shared_ptr<Game>& game)
{
	mPosition = { 0.0f, 0.0f, 0.0f };
	mScale = { 1.0f, 1.0f, 1.0f };
	mRotation = { 0.0f, 0.0f, 0.0f };
	mState = Active;
	mGame = game;
	mGame->addActor(shared_from_this());
}

void Actor::endProccess()
{
	for (auto& component : mComponents) {
		component->endProccess();
	}
}

void Actor::setState(State state)
{
	mState = state;
}

void Actor::setPosition(const XMFLOAT3& position)
{
	mPosition = position;
}

void Actor::setScale(const XMFLOAT3& scale)
{
	mScale = scale;
}

void Actor::setRotation(const XMFLOAT3& rotation)
{
	mRotation = rotation;
}

void Actor::setXPos(float x)
{
	mPosition.x = x;
}

void Actor::setYPos(float y)
{
	mPosition.y = y;
}

void Actor::setZPos(float z)
{
	mPosition.z = z;
}

void Actor::setXRot(float x)
{
	mRotation.x = x;
}

void Actor::setYRot(float y)
{
	mRotation.y = y;
}

void Actor::setZRot(float z)
{
	mRotation.z = z;
}

Actor::State Actor::getState()
{
	return mState;
}

XMFLOAT3 Actor::getPosition() const
{
	return mPosition;
}

XMFLOAT3 Actor::getScale() const
{
	return mScale;
}

XMFLOAT3 Actor::getRotation() const
{
	return mRotation;
}

std::shared_ptr<Game> Actor::getGame()
{
	return mGame;
}

void Actor::addComponent(const std::shared_ptr<Component>& component)
{
	mComponents.emplace_back(component);
	std::sort(mComponents.begin(), mComponents.end(),
		[](const std::shared_ptr<Component>& a, const std::shared_ptr<Component>& b) {
			return a->getUpdateOrder() < b->getUpdateOrder();
		});
}

void Actor::removeComponent(const std::shared_ptr<Component>& component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end()) {
		mComponents.erase(iter);
	}
}
