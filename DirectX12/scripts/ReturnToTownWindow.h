#pragma once
#include "Object.h"
class SpriteComponent;

class ReturnToTownWindow : public Object
{
public:
	ReturnToTownWindow(class DungeonScene& scene, class Player& player);
	DECLARE_CLASS_NAME(ReturnToTownWindow)
	void updateActor() override;
	void inputActor() override;

	void applyComponentLabel() override;

private:
	void startTransitScene();
	void doSelectedAction();
	void deleteWindow();

	class DungeonScene& mScene;
	class TurnObserver& mObserver;
	class Player& mPlayer;

	const int mMaxIndex;
	const float mArrowMoveLength;
	int mSelectedIndex;
	bool isTransitting;

	SpriteComponent* mArrow;
	
};

