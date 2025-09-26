#include "Game.h"
#include "Actor.h"
#include "Player.h"
#include "UI.h"
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

	//ビューマトリックス		
	float h = 0.5f;
	XMFLOAT3 eye = { 0, h, -5 }, focus = { 0, 0, 0 }, up{ 0, 1, 0 };
	XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
	//プロジェクションマトリックス
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, mGraphic->getAspect(), 1.0f, 50.0f);
	XMMATRIX viewProj = view * proj;
	mGraphic->updateViewProj(viewProj);

	//ライトベクトル
	XMFLOAT4 lightPos = { 0, 1, -1, 0 };
	XMVECTOR v = XMLoadFloat4(&lightPos);
	XMVECTOR n = XMVector3Normalize(v);
	XMStoreFloat4(&lightPos, n);
	mGraphic->updateLightPos(lightPos);

	const char* fbx[] = {
		"assets\\Alicia\\FBX\\Alicia.FBX",
		"assets\\Alicia\\FBX\\Alicia.FBX",
	};

	const char* text[] = {
		"assets\\Alicia\\FBX\\Alicia.txt",
		"assets\\Alicia\\FBX\\Alicia.txt",
	};

#if 0
	//FBX→テキスト変換
	FBXConverter fbxConverter;
	const int fbxNum = _countof(fbx);
	int i;
	for (i = 0; i < fbxNum; i++) {
		fbxConverter.fbxToTxt(fbx[i], text[i], 0.01f, 0.01f, 0.01f, 0, 2, 1);
	}
#endif
	//タイマー初期化
	initDeltaTime();

	std::shared_ptr<Player> player = createActor<Player>(shared_from_this());
	player->setPosition({ -4.0f, 0.0f, 0.0f });

	std::shared_ptr<UI> ui = createActor<UI>(shared_from_this());

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
	if (GetAsyncKeyState('A')) {
		std::shared_ptr<Player> actor = createActor<Player>(shared_from_this());
		actor->setPosition({ -4.0f + 1.0f * mActors.size(), 0.0f, 0.0f});
	}

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
