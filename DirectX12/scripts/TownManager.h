#pragma once
#include <string>
#include <stack>
#include "Actor.h"

class Game;
class SpriteComponent;

class TextWindow : public Actor
{
public:
	TextWindow(Game* game, std::string windowName, float zDepth);
	void inputActor() override;


	int getSelectedIndex();
	bool getIsActive();

	void setMaxIndex(int maxIndex);
	void setActive(bool active);

private:
	SpriteComponent* mArrow;
	float mArrowMoveLength;	//矢印の移動距離
	int mSelectedIndex;	//選択されているメニューのインデックス

	int mMaxIndex;	//メニューの最大インデックス

	bool isActive;
};

class BackGround : public Actor
{
public:
	BackGround(Game* game);
};

class TownManager
{
public:
	TownManager(Game* game);

	void update();
	void input();

private:
	Game* mGame;
	bool isTown;
	TextWindow* mMainMenu;
	TextWindow* mSubMenu;
	std::stack<TextWindow*> mMenuStack; //アクティブなメニューを管理
};

