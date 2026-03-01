#pragma once

enum class SceneType {
	TITLE,
	MAP,
};

class SceneManager
{
public:
	SceneManager();
	void setScene(SceneType scene);
	SceneType getCurrentScene();

private:
	SceneType mCurrentScene;
};

