#include "Component.h"
#include "Actor.h"

Component::Component()
{
}

Component::~Component()
{
}

void Component::init(const std::shared_ptr<Actor>& owner, int updateOrder)
{
	mOwner = owner;
	mUpdateOrder = updateOrder;

	mOwner->addComponent(shared_from_this());
}

void Component::updateComponent()
{
}