#include "ReturnToTownWindow.h"
#include "DungeonScene.h"
#include "TurnObserver.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "AudioManager.h"
#include "input.h"
#include "Player.h"


ReturnToTownWindow::ReturnToTownWindow(DungeonScene& scene, Player& player)
	:Object(scene, "ReturnToTownWindow"),
	mScene(scene),
	mObserver(scene.getTurnObserver()),
	mPlayer(player),
	mMaxIndex(2),
	mArrowMoveLength(181.0f),
	mSelectedIndex(0),
	isTransitting(false),
	mArrow(nullptr)
{
	mObserver.stop();

	addComponentLabel("Arrow", "SpriteComponent");

	applyComponentLabel();
}

void ReturnToTownWindow::updateActor()
{
	if (isTransitting && mScene.getGame().getGraphic().isFinishedFade())
	{
		mScene.returnToTown();
	}
}

void ReturnToTownWindow::inputActor()
{
	if (isKeyJustPressed(VK_RIGHT) || isKeyJustPressed('D')) {
		if (mSelectedIndex >= mMaxIndex - 1) return;
		mSelectedIndex++;
		mArrow->movePosition(XMFLOAT2(mArrowMoveLength, 0.0f));
			mScene.getGame().getAudioManager().playSE("UI_MOVE1");
	}
	else if (isKeyJustPressed(VK_LEFT) || isKeyJustPressed('A')) {
		if (mSelectedIndex <= 0) return;
		mSelectedIndex--;
		mArrow->movePosition(XMFLOAT2(-mArrowMoveLength, 0.0f));
			mScene.getGame().getAudioManager().playSE("UI_MOVE1");
	}

	if (isKeyJustPressed(VK_RETURN) || isKeyJustPressed('K')) {
		doSelectedAction();
	}

	if (isKeyJustPressed(VK_ESCAPE))
	{
		deleteWindow();
	}

}

void ReturnToTownWindow::applyComponentLabel()
{
	mArrow = static_cast<SpriteComponent*>(mComponentLabels["Arrow"].pComponent);
}

void ReturnToTownWindow::doSelectedAction()
{
	switch (mSelectedIndex) {
	case 0:
		startTransitScene();
		break;
	case 1:
		deleteWindow();
	}

}

void ReturnToTownWindow::deleteWindow()
{
	mState = State::Dead;
	mPlayer.continueAct();
	mObserver.start();
}

void ReturnToTownWindow::startTransitScene()
{
	isTransitting = true;
	mScene.getGame().getGraphic().startFadeOut(1.0f);
	mScene.getGame().getAudioManager().playSE("ESCAPE");

}
