#pragma once
#include <string>
#include "Actor.h"

class Game;
class SpriteComponent;

class TextWindow : public Actor
{
public:
	TextWindow(Game* game, std::string windowName, int updateOrder);
	void inputActor() override;


	int getSelectedIndex();
	bool getIsSelected();
	bool getIsActive();

	void resetSelected();
	void setMaxIndex(int maxIndex);
	void setActive(bool active);

private:
	SpriteComponent* mArrow;
	float mArrowMoveLength;	//矢印の移動距離
	int mSelectedIndex;	//選択されているメニューのインデックス
	bool isSelected;	//選択されているか

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
};

