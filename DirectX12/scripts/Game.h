#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <DirectXMath.h>
#include <cassert>

#include "input.h"
#include "BIN_FILE12.h"
#include "Definition.h"
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
class PointLightComponent;
class SpotLightComponent;
class Player;
class TextComponent;
class MapManager;
class EnemyComponent;
class DamageTextManager;

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
	//スプライトの追加
	void addSprite(const std::shared_ptr<SpriteComponent>& mesh);
	void removeSprite(const std::shared_ptr<SpriteComponent>& mesh);
	//点光源の追加
	void addPointLight(const std::shared_ptr<PointLightComponent>& light);
	void removePointLight(const std::shared_ptr<PointLightComponent>& light);
	//スポットライトの追加
	void addSpotLight(const std::shared_ptr<SpotLightComponent>& light);
	void removeSpotLight(const std::shared_ptr<SpotLightComponent>& light);
	//テキストの追加
	void addText(const std::shared_ptr<TextComponent>& text);
	void removeText(const std::shared_ptr<TextComponent>& text);
	//エネミーの追加
	void addEnemy(const std::shared_ptr<EnemyComponent>& enemy);
	void removeEnemy(const std::shared_ptr<EnemyComponent>& enemy);

	//ゲッター
	std::shared_ptr<Graphic> getGraphic();
	std::shared_ptr<std::vector<std::shared_ptr<EnemyComponent>>> getEnemies();
	std::shared_ptr<MapManager> getMapManager();
	std::shared_ptr<DamageTextManager> getDamageTextManager();
	std::shared_ptr<EnemyComponent> getEnemyFromIndexPos(const std::vector<int>& indexPos);
	std::shared_ptr<EnemyComponent> getEnemyFromIndexPos(int index);


private:
	//グラフィック
	std::shared_ptr<Graphic> mGraphic;
	//メッシュ配列
	std::vector<std::shared_ptr<MeshComponent>> mMeshes;
	std::vector<std::shared_ptr<SpriteComponent>> mSprites;
	//テキスト配列
	std::vector<std::shared_ptr<TextComponent>> mTexts;
	//ライト配列
	std::vector<std::shared_ptr<PointLightComponent>> mPointLights;
	std::vector<std::shared_ptr<SpotLightComponent>> mSpotLights;
	//アクター配列
	std::vector<std::shared_ptr<Actor>> mActors;
	std::vector<std::shared_ptr<Actor>> mPendingActors;
	//キャラクター配列
	std::shared_ptr<std::vector<std::shared_ptr<EnemyComponent>>> mEnemies = nullptr; 
	//ダメージエフェクト用
	std::shared_ptr<DamageTextManager> mDamageTextManager;

	//マップ関連
	std::shared_ptr<MapManager> mMapManager;
	bool mUpdatingActors;

	//プレイヤー
	std::shared_ptr<Player> mPlayer;

	//ループ内処理
	void input();
	void update();
	void draw();

};