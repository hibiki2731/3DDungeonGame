#pragma once
#include <random>
#include "CharacterComponent.h"
#include "MeshComponent.h"

class Player;

class EnemyComponent : public CharacterComponent
{
public:
	void initComponent() override;
	void inputComponent() override;
	void updateComponent() override;
	void endProccess() override;

	void setMesh(const std::shared_ptr<MeshComponent>& mesh);
	void setEnemyTtype(int type);
	void activate();

	void startFlash();

	//getter
	XMFLOAT3 getPosition();

private:
	void updateFlash(); //ダメージ時の点滅を更新
	void move();
	void attack();
	void finishAct();

	std::shared_ptr<MeshComponent> mMesh;

	//ダメージ時の点滅
	float mFlashTimer;
	float mFlashDuration;

	//移動用
	XMFLOAT3 mTargetPos;
	float mMoveSpeed;
	std::mt19937 mt; //メルセンヌ・ツイスタ法の生成器
	bool isMoving;

	//敵タイプ
	int mEnemyType;

	//ターン制御
	bool isActive;

};

