#include "SceneManager.h"

SceneManager::SceneManager()
{
	mCurrentScene = SceneType::TITLE;
}

void SceneManager::setScene(SceneType scene)
{
	mCurrentScene = scene;
}

SceneType SceneManager::getCurrentScene()
{
	return mCurrentScene;
}

