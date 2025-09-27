#include "UI.h"
#include "Game.h"
#include "Component.h"
#include "Anime2DComponent.h"
#include "SpriteComponent.h"

void UI::initActor()
{
	mTextureIndex = 0;
	counter = 0;
	mAnime = createComponent<Anime2DComponent>(shared_from_this());
	mAnime->create("assets\\josei_04_akamafu\\josei_04_akamafu\\PNG\\josei_04_.png", 4);
	setScale({1.0f / getGame()->getGraphic()->getAspect() * 0.5f, 1.0f * 1.1f, 1.0f});

	auto mSprite = createComponent<SpriteComponent>(shared_from_this());
	mSprite->create("assets\\white.png");
}	

void UI::updateActor()
{
	counter++;
	if (counter % 30 == 0){
		mTextureIndex++;
		if (mTextureIndex >= mAnime->getTextureNum()) {
			mTextureIndex = 0;
		}
		mAnime->setTextureIndex(mTextureIndex);
		
	}
}