#include "Game.h"
#include "Actor.h"
#include "Player.h"
#include "UI.h"
#include "Camera.h"
#include "Light.h"
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "RenderComponent.h"


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
	mGraphic = std::make_shared<Graphic>();
	mGraphic->init();
	mGraphic->clearColor(0.25f, 0.5f, 0.9f);

	std::shared_ptr<Camera> camera = createActor<Camera>(shared_from_this());


	const char* fbx[] = { 
		"assets\\Models\\FBX format\\room-corner.fbx",
	};

	const char* text[] = { 
		"assets\\Models\\FBX format\\gate.txt",
	};

#if 1
	//FBX→テキスト変換
	FBXConverter fbxConverter;
	const int fbxNum = _countof(fbx);
	int i;
	for (i = 0; i < fbxNum; i++) {
		fbxConverter.fbxToTxt(fbx[i], text[i], 0.005f, 0.005f, 0.005f, 0, 1, 2);
	}
#endif
	//タイマー初期化
	initDeltaTime();

	std::shared_ptr<Player> player = createActor<Player>(shared_from_this());

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

std::shared_ptr<Graphic> Game::getGraphic()
{
	return mGraphic;
}

void Game::input()
{	




	for (auto& actor : mActors) {
		actor->input();
	}
}

void Game::update()
{
	mUpdatingActors = true;
	//アクターの更新処理
	for (auto& actor : mActors) {
		actor->update();
	}
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

}

void Game::draw()
{
	//描画
	//3D描画
	mGraphic->beginRender();
	mGraphic->setRenderType(Graphic::RENDER3D);
	for (auto& mesh : mMeshes) {
		mesh->draw();
	}

	//2D描
	mGraphic->setRenderType(Graphic::RENDER2D);
	for (auto& sprite : mSprites) {
		sprite->draw();
	}
	mGraphic->endRender();

}
