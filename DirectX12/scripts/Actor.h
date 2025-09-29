#pragma once
#include <concepts>
#include <memory>
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

//前方宣言
class Game;
class Component;

class Actor : public std::enable_shared_from_this<Actor>
{
public:

	enum State {
		Active,
		Paused,
		Dead
	};

	Actor() {};
	virtual ~Actor() {};

	//入力処理
	void input();
	virtual void inputActor() {};

	//更新関数
	void update();
	//コンポーネントを更新
	void updateComponents();
	//アクター独自の更新処理
	virtual void updateActor() {};

	//コンポーネントの追加/削除
	void addComponent(const std::shared_ptr<Component>& component);
	void removeComponent(const std::shared_ptr<Component>& component);
	//初期化
	void init(const std::shared_ptr<Game>& game);
	//アクター独自の初期化
	virtual void initActor() {};

	//セッター
	void setState(State state);
	void setPosition(const XMFLOAT3& position);
	void setScale(const XMFLOAT3& scale);
	void setRotation(const XMFLOAT3& rotation);

	void setXPos(float x);
	void setYPos(float y);
	void setZPos(float z);
	void setXRot(float x);
	void setYRot(float y);
	void setZRot(float z);


	//ゲッター
	State getState();
	XMFLOAT3 getPosition() const;
	XMFLOAT3 getScale() const;
	XMFLOAT3 getRotation() const;
	std::shared_ptr<Game> getGame();

private:
	//newを禁止にする
	static void* operator new(size_t size);

protected:
	//アクターの状態
	State mState;

	//コンポーネント
	std::vector<std::shared_ptr<Component>> mComponents;
	std::shared_ptr<Game> mGame;

	//ベクトル
	XMFLOAT3 mPosition;
	XMFLOAT3 mScale;
	XMFLOAT3 mRotation;
};


//Actorの作成
//テンプレートの制約
template <typename ActorType> concept isActor =	std::is_base_of<Actor, ActorType>::value == true;
//initを持つクラスの生成関数
template <isActor ActorType>
std::shared_ptr<ActorType> createActor(const std::shared_ptr<Game>& game) {
	std::shared_ptr<ActorType> actor = std::make_shared<ActorType>();
	actor->init(game);
	actor->initActor();
	return actor;
};
