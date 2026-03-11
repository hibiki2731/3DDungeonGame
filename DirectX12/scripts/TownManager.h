#pragma once
#include <string>
#include <stack>
#include "Actor.h"

class Game;
class SpriteComponent;

class Menu : public Actor
{
public:
	Menu(Game* game, std::string windowName, float zDepth);
	//input
	void inputMenu();
	virtual void updateMenu() {};

protected:
	int mMaxIndex;	//メニューの最大インデックス
	int mSelectedIndex;	//選択されているメニューのインデックス

private:
	void initComponent(std::string windowName, float zDepth);

	SpriteComponent* mArrow;
	float mArrowMoveLength;	//矢印の移動距離
};

class MainMenu : public Menu {
public:
	MainMenu(Game* game, float zDepth);
	void updateMenu() override;
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
	
	void pushMenu(Menu* menu);
	void popMenu();

	void exitStatusMenu();

private:
	Game* mGame;
	BackGround* mBg;
	bool isTown;
	bool isSelected;
	bool isSelecetdStatus;
	bool isStatusMenu;
	std::stack<Menu*> mMenuStack; //アクティブなメニューを管理
};

