#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <DirectXMath.h>
#include <cassert>

#include "input.h"
#include "BIN_FILE12.h"
#include "Buffer.h"
#include "FBXConverter.h"
#include "timer.h"
#include "Graphic.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

//前方宣言
class Actor;
class Component;
class RenderComponent;
class MeshComponent;
class SpriteComponent;
class LightComponent;
class Player;

class Game : public std::enable_shared_from_this<Game>{
public:
	Game();
	~Game();
	//ループの開始
	void runLoop();
	//終了処理
	int endProcess();
	//初期化
	void init();

	//アクターの追加
	void addActor(const std::shared_ptr<Actor>& actor);
	void removeActor(const std::shared_ptr<Actor>& actor);
	//メッシュの追加
	void addMesh(const std::shared_ptr<MeshComponent>& mesh);
	void removeMesh(const std::shared_ptr<MeshComponent>& mesh);
	void addSprite(const std::shared_ptr<SpriteComponent>& mesh);
	void removeSprite(const std::shared_ptr<SpriteComponent>& mesh);
	void addLight(const Light& light, int index);

	//ゲッター
	std::shared_ptr<Graphic> getGraphic();

private:

	//ループ内処理
	void input();
	void update();
	void draw();

	//グラフィック
	std::shared_ptr<Graphic> mGraphic;
	//メッシュ配列
	std::vector<std::shared_ptr<MeshComponent>> mMeshes;
	std::vector<std::shared_ptr<SpriteComponent>> mSprites;
	//ライト配列
	//std::vector<std::shared_ptr<LightComponent>> mLights;
	Light mLights[4];
	//アクター配列
	std::vector<std::shared_ptr<Actor>> mActors;
	std::vector<std::shared_ptr<Actor>> mPendingActors;

	bool mUpdatingActors;
	
};