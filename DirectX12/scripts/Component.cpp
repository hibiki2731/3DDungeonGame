#include "Component.h"
#include "Actor.h"

Component::Component(Actor& owner, int updateOrder)
	: mOwner(owner),
	mUpdateOrder(updateOrder)
{
	mOwnerIsDead = false;
}

void Component::dead()
{
	mOwnerIsDead = true;
}
