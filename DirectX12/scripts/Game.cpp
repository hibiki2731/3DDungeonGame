#include "Game.h"
#include "Actor.h"
#include "Player.h"
#include "UI.h"
#include "Player.h"
#include "MessageWindow.h"
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "PointLightComponent.h"
#include "PointLight.h"
#include "RockObject.h"
#include "TextComponent.h"
#include "Enemy.h"
#include "MapManager.h"
#include "EnemyComponent.h"
#include "DamageText.h"
#include "Graphic.h"

#define DEBUG



Game::Game(){
	mUpdatingActors = false;
}

Game::~Game() {
}

void Game::runLoop()
{
	while (!mGraphic->quit()) {
		setDeltaTime();

		input();
		update();
		draw();
	}
}

int Game::endProcess()
{
	mGraphic->waitGPU();
	mGraphic->closeEventHandle();
	return mGraphic->msg_wparam();
}

void Game::init() {
	mGraphic = std::make_unique<Graphic>(this);
	mGraphic->init();
	mGraphic->clearColor(0.25f, 0.5f, 0.9f);

	//FBXファイルとテキストファイルのパス
	const char* fbx[] = { 
		"assets\\rockObj\\rockWall.fbx",
		"assets\\rockObj\\rockFloor.fbx",
		"assets\\Grass\\grass.fbx",
		"assets\\slime.fbx",
	};

	const char* text[] = { 
		"assets\\rockObj\\rockWall.txt",
		"assets\\rockObj\\rockFloor.txt",
		"assets\\Grass\\grass.txt",
		"assets\\slime.txt",
	};

#ifdef DEBUG
	//FBX→テキスト変換
	FBXConverter fbxConverter;
	const int fbxNum = _countof(fbx);
	int i;
	//フリー素材
	for (i = 0; i < fbxNum; i++) {
		fbxConverter.fbxToTxt(fbx[i], text[i], 1.0f, 1.0f, 1.0f, 0, 1, 2); //横、縦、奥行
		
	}
#endif


	//タイマー初期化
	initDeltaTime();

	//assetManagerの初期化 meshComponentを作成する前に初期化
	mAssetManager = std::make_unique<AssetManager>(mGraphic.get());

	//mapの生成
	mMapManager = std::make_unique<MapManager>(this);
	mMapManager->setStage(Stage::MAP1);
	mMapManager->createMap();

	//アクター作成例
	auto messageWindow = std::make_unique<MessageWindow>(this);
	messageWindow->setPlayer(mPlayer); //デバッグ用
	addActor(std::move(messageWindow));

	//itemManagerの初期化
	mItemManager = std::make_unique<ItemManager>();

	//damageTextの初期化
	mDamageTextManager = std::make_unique<DamageTextManager>(this);

}

void Game::addActor(std::unique_ptr<Actor> actor)
{
	//アクターの更新中なら待ちに追加
	if (mUpdatingActors) {
		mPendingActors.emplace_back(std::move(actor));
	}
	else {
		mActors.emplace_back(std::move(actor));
	}
}

void Game::removeActor(std::unique_ptr<Actor>& actor)
{
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end()) {
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	//除去時の処理
	actor->endProccess();

	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end()) {
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::addMesh(MeshComponent* mesh)
{
	mMeshes.emplace_back(mesh);
}

void Game::removeMesh(MeshComponent* mesh)
{
	auto iter = std::find(mMeshes.begin(), mMeshes.end(), mesh);
	if (iter != mMeshes.end()) {
		std::iter_swap(iter, mMeshes.end() - 1);
		mMeshes.pop_back();
	}
}
void Game::addSprite(SpriteComponent* sprite)
{
	mSprites.emplace_back(sprite);
}

void Game::removeSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	if (iter != mSprites.end()) {
		std::iter_swap(iter, mSprites.end() - 1);
		mSprites.pop_back();
	}
}

void Game::addPointLight(PointLightComponent* light)
{
	if (mPointLights.size() > MAX_LIGHT_NUM) return;
	mPointLights.emplace_back(light);
}

void Game::removePointLight(PointLightComponent* light)
{
	mPointLights.erase(std::remove(mPointLights.begin(), mPointLights.end(), light), mPointLights.end());
}

void Game::addSpotLight(SpotLightComponent* light)
{
	if (mSpotLights.size() > MAX_LIGHT_NUM) return;
	mSpotLights.emplace_back(light);
}

void Game::removeSpotLight(SpotLightComponent* light)
{
	mSpotLights.erase(std::remove(mSpotLights.begin(), mSpotLights.end(), light), mSpotLights.end());
}

void Game::addText(TextComponent* text)
{
	mTexts.emplace_back(text);
}

void Game::removeText(TextComponent* fontText)
{
	mTexts.erase(std::remove(mTexts.begin(), mTexts.end(), fontText), mTexts.end());
}

void Game::addEnemy(EnemyComponent* enemy)
{
	mEnemies.emplace_back(enemy);
}

void Game::removeEnemy(EnemyComponent* enemy)
{
	auto iter = std::find(mEnemies.begin(), mEnemies.end(), enemy);
	if (iter != mEnemies.end()) {
		std::iter_swap(iter, mEnemies.end() - 1);
		mEnemies.pop_back();
	}
}

void Game::setPlayer(Player* player)
{
	mPlayer = player;
}

void Game::activateEnemies()
{
	for (auto enemy : mEnemies) {
		enemy->activate();
	}
}

Graphic* Game::getGraphic()
{
	return mGraphic.get();
}

std::vector<EnemyComponent*>& Game::getEnemies()
{
	return mEnemies;
}

MapManager* Game::getMapManager()
{
	return mMapManager.get();
}

DamageTextManager* Game::getDamageTextManager()
{
	return mDamageTextManager.get();
}

EnemyComponent* Game::getEnemyFromIndexPos(int x, int y)
{
	for (auto enemy : mEnemies) {
		std::vector<int> charIndexPos = enemy->getIndexPos();
		if (charIndexPos[0] == x && charIndexPos[1] == y) {
			return enemy;
		}
	}
	return nullptr;
}

EnemyComponent* Game::getEnemyFromIndexPos(int index)
{
	int mapSize = mMapManager->getMapSize();
	int x = index % mapSize;
	int y = index / mapSize;
	for (auto enemy : mEnemies) {
		std::vector<int> charIndexPos = enemy->getIndexPos();
		if (charIndexPos[0] == x && charIndexPos[1] == y) {
			return enemy;
		}
	}

	return nullptr;
}

AssetManager* Game::getAssetManager()
{
	return mAssetManager.get();
}

std::vector<PointLightComponent*>& Game::getPointLights()
{
	return mPointLights;
}

std::vector<SpotLightComponent*>& Game::getSpotLights()
{
	return mSpotLights;
}

Player* Game::getPlayer()
{
	return mPlayer;
}

ItemManager* Game::getItemManager()
{
	return mItemManager.get();
}

void Game::input()
{

	for (auto& actor : mActors) {

		actor->input();
	}

	//デバック用
	if (GetAsyncKeyState('P')) {
		auto slime = std::make_unique<Enemy>(this, static_cast<float>(MAPTIPSIZE * 5.0f), static_cast<float>(MAPTIPSIZE * 5.0f));
		addActor(std::move(slime));
	}
	if (GetAsyncKeyState('O')) {
		mMapManager->moveToPlayerTurn();
	}

}

void Game::update()
{
	mUpdatingActors = true;
	//アクターの更新処理
	for (auto& actor : mActors) {
		actor->update();
	}

	for (auto enemy : mEnemies) {
		enemy->updateActiveProcess();
	}
	//敵配列をプレイヤーに近い順にソート
	std::sort(mEnemies.begin(), mEnemies.end(), [](auto const lenemy, auto const renemy){
		return lenemy->getDist() < renemy->getDist();
		});

	mUpdatingActors = false;

	for (auto& pending : mPendingActors) {
		mActors.emplace_back(std::move(pending));
	}
	mPendingActors.clear();

	//死んだアクターを一次配列に追加
	std::vector<std::unique_ptr<Actor>> deadActors;
	for (auto& actor : mActors) {
		if (actor->getState() == Actor::Dead) {
			actor->endProccess();
			deadActors.emplace_back(std::move(actor));
		}
	}
	//元配列に残ったnullptrを削除
	std::erase_if(mActors, [](const std::unique_ptr<Actor>& actor) {
		return actor == nullptr;
		});

	//Base3DDataの更新
	mGraphic->updateBase3DData();
	//ダメージテキストの更新
	mDamageTextManager->update();
	//ターンの変更
	mMapManager->update();

}

void Game::draw()
{
	//描画
	//3D描画
	mGraphic->begin3DRender();
	mGraphic->setRenderType(Graphic::RENDER_3D);
	for (auto& mesh : mMeshes) {
		mesh->draw();
	}

	//2D描画
	mGraphic->setRenderType(Graphic::RENDER_2D);
	for (auto& sprite : mSprites) {
		sprite->draw();
	}

	//ダメージエフェクト
	mGraphic->setRenderType(Graphic::RENDER_DT);
	mDamageTextManager->draw();

	mGraphic->end3DRender();

	////テキスト描画
	mGraphic->begin2DRender();
	for (auto& text : mTexts) {
		text->draw();
	}

	mGraphic->end2DRender();


	mGraphic->moveToNextFrame();


}
