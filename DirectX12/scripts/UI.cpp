#include "UI.h"
#include "Game.h"
#include "Component.h"
#include "SpriteComponent.h"

void UI::initActor()
{
	mTextureIndex = 0;
	counter = 0;
	mSprite = createComponent<SpriteComponent>(shared_from_this());
	mSprite->create("assets\\josei_04_akamafu\\josei_04_akamafu\\PNG\\josei_04_.png", 4);
	setScale({1.0f / getGame()->getGraphic()->getAspect() * 0.5f, 1.0f * 1.1f, 1.0f});
}

void UI::updateActor()
{
	counter++;
	if (counter % 30 == 0){
		mTextureIndex++;
		if (mTextureIndex >= mSprite->getTextureNum()) {
			mTextureIndex = 0;
		}
		mSprite->setTextureIndex(mTextureIndex);
		
	}
}