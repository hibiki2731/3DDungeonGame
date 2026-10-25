#pragma once
#include <memory>
#include "CharacterComponent.h"
#include "MeshComponent.h"
#include "EnemyMoveStrategy.h"
#include "EnemyAttackStrategy.h"

//前方宣言
class Player;
class DungeonScene;

class EnemyComponent : public CharacterComponent
{
public:
	EnemyComponent(Actor& owner, DungeonScene& scene);
	~EnemyComponent();
	DECLARE_COMPONENT_NAME(EnemyComponent)

	void updateComponent() override;	//毎フレームの更新処理
	void endProcess() override;			//コンポーネントが破棄される際の処理
	void startAct();					//エネミーの行動を開始する
	void startFlash();					//ダメージを受けたときの点滅を開始
	void lookAt(const std::array<int, 2>& targetIndex);	//隣接するマスの方向を向く

	//setter
	void setMesh(MeshComponent* mesh);		//メッシュの設定
	void setMoveStrategy(std::unique_ptr<EnemyMoveStrategy> strategy); //行動パターンの設定
	void setAttackStrategy(std::unique_ptr<EnemyAttackStrategy> strategy); //攻撃パターンの設定
	void setDropMoney(int money);			//エネミー撃破時に落とすお金の量を設定
	void setName(const std::string& name);	//エネミーの名前を設定
	void setEnemyID(const std::string& enemyID) { mEnemyID = enemyID; }	//エネミーID(enemyData.jsonのキー)を設定

	//getter
	XMFLOAT3 getPosition();	//位置の取得
	int getDist();			//プレイヤーとの距離を取得
	const std::string& getName() { return mName; } //エネミーの名前を取得
	const std::string& getEnemyID() const { return mEnemyID; } //エネミーIDを取得
	bool isActing() {return isActive;}

private:
	void attack();									//プレイヤーが隣接していたら攻撃する
	void move();									//移動のためのデータ更新を行う
	void updateFlash();							//ダメージ時の点滅を更新
	void finishAct();								//行動の終了処理を行う
	void deadProcess();								//死亡時の処理

	std::string		mName; 				//エネミーの名前
	std::string		mEnemyID;			//エネミーID(enemyData.jsonのキー)
	MeshComponent*	mMesh;			    //エネミーに設定されているメッシュ
	int				mDropMoney;			//ドロップするお金
	bool			isActive;			//true : 行動可能  false : 行動済み
	std::unique_ptr<EnemyMoveStrategy> mMoveStrategy;	//行動パターン
	std::unique_ptr<EnemyAttackStrategy> mAttackStrategy;	//攻撃パターン
	int				mDistPlayer; 		//プレイヤーからの距離(マンハッタン距離)、エネミーの更新順序を計算する際に使用
	DungeonScene&	mScene;				//シーン
	//---点滅処理用---
	float		    mFlashTimer;		//ダメージ時の点滅用タイマー
	float			mFlashDuration;		//ダメージ時の点滅の長さ
	//--移動処理用---
	XMFLOAT3		mTargetPos;			//移動先の位置
	float			mMoveSpeed;			//移動スピード
	bool			isMoving;			//移動フラグ　ture : 移動中　false : 停止

};

