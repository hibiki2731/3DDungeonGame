#include "Game.h"
#include "Actor.h"
#include "Player.h"
#include "UI.h"
#include "Player.h"
#include "MessageWindow.h"
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "RenderComponent.h"
#include "PointLightComponent.h"
#include "PointLight.h"
#include "RockObject.h"
#include "TextComponent.h"
#include "Slime.h"
#include "MapManager.h"
#include "EnemyComponent.h"
#include "DamageText.h"
#include "Graphic.h"

#define DEBUG



Game::Game(){
	mUpdatingActors = false;
}

Game::~Game() {}

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
	mGraphic = std::make_unique<Graphic>(shared_from_this());
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
	auto messageWindow = createActor<MessageWindow>(this);
	messageWindow->setPlayer(mPlayer.get()); //デバッグ用
	addActor(std::move(messageWindow));

	//itemManagerの初期化
	mItemManager = std::make_unique<ItemManager>();

	//damageTextの初期化
	mDamageTextManager = std::make_unique<DamageTextManager>(this);

}

void Game::addActor(const std::shared_ptr<Actor>& actor)
{
	//アクターの更新中なら待ちに追加
	if (mUpdatingActors) {
		mPendingActors.emplace_back(actor);
	}
	else {
		mActors.emplace_back(actor);
	}
}

void Game::removeActor(const std::shared_ptr<Actor>& actor)
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

void Game::addMesh(const std::shared_ptr<MeshComponent>& mesh)
{
	mMeshes.emplace_back(mesh);
}

void Game::removeMesh(const std::shared_ptr<MeshComponent>& mesh)
{
	auto iter = std::find(mMeshes.begin(), mMeshes.end(), mesh);
	if (iter != mMeshes.end()) {
		std::iter_swap(iter, mMeshes.end() - 1);
		mMeshes.pop_back();
	}
}
void Game::addSprite(const std::shared_ptr<SpriteComponent>& sprite)
{
	mSprites.emplace_back(sprite);
}

void Game::removeSprite(const std::shared_ptr<SpriteComponent>& sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	if (iter != mSprites.end()) {
		std::iter_swap(iter, mSprites.end() - 1);
		mSprites.pop_back();
	}
}

void Game::addPointLight(const std::shared_ptr<PointLightComponent>& light)
{
	if (mPointLights.size() > MAX_LIGHT_NUM) return;
	mPointLights.push_back(light);
}

void Game::removePointLight(const std::shared_ptr<PointLightComponent>& light)
{
	mPointLights.erase(std::remove(mPointLights.begin(), mPointLights.end(), light), mPointLights.end());
}

void Game::addSpotLight(const std::shared_ptr<SpotLightComponent>& light)
{
	if (mSpotLights.size() > MAX_LIGHT_NUM) return;
	mSpotLights.push_back(light);
}

void Game::removeSpotLight(const std::shared_ptr<SpotLightComponent>& light)
{
	mSpotLights.erase(std::remove(mSpotLights.begin(), mSpotLights.end(), light), mSpotLights.end());
}

void Game::addText(const std::shared_ptr<TextComponent>& text)
{
	mTexts.push_back(text);
}

void Game::removeText(const std::shared_ptr<TextComponent>& fontText)
{
	mTexts.erase(std::remove(mTexts.begin(), mTexts.end(), fontText), mTexts.end());
}

void Game::addEnemy(const std::shared_ptr<EnemyComponent>& enemy)
{
	mEnemies->push_back(enemy);
}

void Game::removeEnemy(const std::shared_ptr<EnemyComponent>& enemy)
{
	auto iter = std::find(mEnemies->begin(), mEnemies->end(), enemy);
	if (iter != mEnemies->end()) {
		std::iter_swap(iter, mEnemies->end() - 1);
		mEnemies->pop_back();
	}
}

void Game::setPlayer(const std::shared_ptr<Player>& player)
{
	mPlayer = player;
}

void Game::activateEnemies()
{
	for (auto enemy : *mEnemies) {
		enemy->activate();
	}
}

Graphic* Game::getGraphic()
{
	return mGraphic.get();
}

std::shared_ptr<std::vector<std::shared_ptr<EnemyComponent>>> Game::getEnemies()
{
	return mEnemies;
}

std::shared_ptr<MapManager> Game::getMapManager()
{
	return mMapManager;
}

std::shared_ptr<DamageTextManager> Game::getDamageTextManager()
{
	return mDamageTextManager;
}

std::shared_ptr<EnemyComponent> Game::getEnemyFromIndexPos(int x, int y)
{
	for (auto& enemy : *mEnemies) {
		std::vector<int> charIndexPos = enemy->getIndexPos();
		if (charIndexPos[0] == x && charIndexPos[1] == y) {
			return enemy;
		}
	}
	return nullptr;
}

std::shared_ptr<EnemyComponent> Game::getEnemyFromIndexPos(int index)
{
	int mapSize = mMapManager->getMapSize();
	int x = index % mapSize;
	int y = index / mapSize;
	for (auto& enemy : *mEnemies) {
		std::vector<int> charIndexPos = enemy->getIndexPos();
		if (charIndexPos[0] == x && charIndexPos[1] == y) {
			return enemy;
		}
	}

	return nullptr;
}

std::shared_ptr<AssetManager> Game::getAssetManager()
{
	return mAssetManager;
}

std::vector<std::shared_ptr<PointLightComponent>> Game::getPointLights()
{
	return mPointLights;
}

std::vector<std::shared_ptr<SpotLightComponent>> Game::getSpotLights()
{
	return mSpotLights;
}

std::shared_ptr<Player> Game::getPlayer()
{
	return mPlayer;
}

std::shared_ptr<ItemManager> Game::getItemManager()
{
	return mItemManager;
}

void Game::input()
{

	for (auto& actor : mActors) {
		if (actor == nullptr) {
			continue;
		}

		actor->input();
	}

	//デバック用
	if (GetAsyncKeyState('P')) {
		auto slime = createActor<Slime>(shared_from_this());
		slime->setPosition(XMFLOAT3(MAPTIPSIZE * 5.0f, 0.0f, MAPTIPSIZE * 5.0f));
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

	for (auto& enemy : *mEnemies) {
		enemy->updateActiveProcess();
	}
	//敵配列をプレイヤーに近い順にソート
	std::sort((*mEnemies).begin(), (*mEnemies).end(), [](auto const& lenemy, auto const& renemy){
		return lenemy->getDist() < renemy->getDist();
		});

	mUpdatingActors = false;

	for (auto pending : mPendingActors) {
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	//死んだアクターを一次配列に追加
	std::vector<std::shared_ptr<Actor>> deadActors;
	for (auto actor : mActors) {
		if (actor->getState() == Actor::Dead) {
			deadActors.emplace_back(actor);
		}
	}

	//死んだアクターを削除
	for (auto actor : deadActors) {
		removeActor(actor);
	}

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
