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

	void setMaxIndex(int maxIndex);

private:
	SpriteComponent* mArrow;
	float mArrowMoveLength;	//矢印の移動距離
	int mSelectedIndex;	//選択されているメニューのインデックス
	int mMaxIndex;	//メニューの最大インデックス
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
private:
	Game* mGame;
	bool isTown;
};

