#pragma once
#include "Component.h"
#include "MapManager.h"
#include <DirectXMath.h>

using namespace DirectX;

enum Direction {
	DOWN = 1,
	RIGHT = 2,
	UP = 4,
	LEFT =8,
};


class CharacterComponent : public Component
{
public:
	CharacterComponent() {};
	~CharacterComponent() {};

	void initComponent() override;
	void inputComponent() override;
	void updateComponent() override;
	void endProccess() override;

	//getter
	int getHP();
	int getPower();
	int getDefense();
	int getDirection();
	bool getAlive();
	XMFLOAT2 getIndexPos();
	int getIndexPosInt();
	std::shared_ptr<MapManager> getMapManager();
	std::shared_ptr<CharacterComponent> getCharacterFromIndexPos(XMFLOAT2 indexPos);
	std::shared_ptr<CharacterComponent> getCharacterFromIndexPos(int index);

	//setter
	void setMaxHP(int maxHP);
	void setHP(int hp);
	void setPower(int power);
	void setDefense(int defense);
	void setDirection(int direction);
	void setIndexPos(XMFLOAT2 indexPos);
	void setIndexPos(int index);

	void giveDamage(int damage);
	void turnRight();
	void turnLeft();


private:
	//ステータス
	int mMaxHP;
	int mHP;
	int mPower;
	int mDefense;
	bool isAlive;

	//向き
	int mDirection; //左上右下 0000

	//位置
	XMFLOAT2 mIndexPos;

	//キャラクター配列
	std::shared_ptr<std::vector<std::shared_ptr<CharacterComponent>>> mCharacters;

	std::shared_ptr<MapManager> mMapManager;
};

