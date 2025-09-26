#include "UI.h"
#include "Game.h"
#include "Component.h"
#include "SpriteComponent.h"

void UI::initActor()
{
	auto sprite = createComponent<SpriteComponent>(shared_from_this());
	sprite->create("assets\\white.png");
	//sprite->setRect({ 0.3f, 0.3f, 0.2f, 0.2f });

	setScale({ 1.0f / getGame()->getGraphic()->getAspect(), 1.0f, 1.0f});
}
