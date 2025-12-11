#include "Game.h"
#include "Actor.h"
#include "Player.h"
#include "UI.h"
#include "Camera.h"
#include "MessageWindow.h"
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "RenderComponent.h"
#include "PointLightComponent.h"
#include "PointLight.h"
#include "RockWall.h"
#include "TextComponent.h"




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

	//カメラ作成
	std::shared_ptr<Camera> camera = createActor<Camera>(shared_from_this());



	const char* fbx[] = { 
		"assets\\Models\\FBX format\\room-corner.fbx",
		"assets\\rock_side\\wall_side.fbx",
		"assets\\desk_refined.fbx",
		"assets\\rockObj\\rockWall.fbx",
		"assets\\rockObj\\rockFloor.fbx",
	};

	const char* text[] = { 
		"assets\\Models\\FBX format\\room-corner.txt",
		"assets\\rock_side\\wall_side.txt",
		"assets\\desk_refined.txt",
		"assets\\rockObj\\rockWall.txt",
		"assets\\rockObj\\rockFloor.txt",
	};

#if 1
	//FBX→テキスト変換
	FBXConverter fbxConverter;
	const int fbxNum = _countof(fbx);
	int i;
	//フリー素材
	fbxConverter.fbxToTxt(fbx[0], text[0], 0.005f, 0.005f, 0.005f, 2, 1, 0); //横、縦、奥行
	//blender
	for (i = 1; i < fbxNum; i++) {
		fbxConverter.fbxToTxt(fbx[i], text[i], 1.0f, 1.0f, 1.0f, 0, 2, 1); //横、縦、奥行
	}
#endif
	//タイマー初期化
	initDeltaTime();

	//アクター作成例
	auto messageWindow = createActor<MessageWindow>(shared_from_this());
	auto wall = createActor<RockWall>(shared_from_this());
	auto wall1 = createActor<RockWall>(shared_from_this());
	auto floor = createActor<RockFloor>(shared_from_this());
	auto floor1 = createActor<RockFloor>(shared_from_this());
	floor1->setPosition(XMFLOAT3(0, 0, 2.0f));
	auto floor2 = createActor<RockFloor>(shared_from_this());
	floor2->setPosition(XMFLOAT3(0, 0, 4.0f));
	auto flor3 = createActor<RockFloor>(shared_from_this());
	flor3->setPosition(XMFLOAT3(2.0f, 0, 2.0f));
	auto flor4 = createActor<RockFloor>(shared_from_this());
	flor4->setPosition(XMFLOAT3(2.0f, 0, 4.0f));
	auto flor5 = createActor<RockFloor>(shared_from_this());
	flor5->setPosition(XMFLOAT3(0, 0, 0));
	auto flor6 = createActor<RockFloor>(shared_from_this());
	flor6->setPosition(XMFLOAT3(2.0f, 0, 0));
	auto wall2 = createActor<RockWall>(shared_from_this());
	wall2->setPosition(XMFLOAT3(0, 0, 4.0f));
	wall2->setYRot(PI / 2.0f);
	wall->setYRot(PI / 2.0f);
	wall1->setPosition(XMFLOAT3(0, 0, 2.0f));
	wall1->setYRot(PI / 2.0f);

	auto pointLight1 = createActor<PointLight>(shared_from_this());
	pointLight1->setPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
	auto pointLight2 = createActor<PointLight>(shared_from_this());
	pointLight2->setPosition(XMFLOAT3(2.0f, 1.0f, 2.0f));
	auto pointLight3 = createActor<PointLight>(shared_from_this());
	pointLight3->setPosition(XMFLOAT3(0.0f, 1.0f, 4.0f));

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
	mTexts.emplace_back(text);
}

void Game::removeText(const std::shared_ptr<TextComponent>& fontText)
{
	mTexts.erase(std::remove(mTexts.begin(), mTexts.end(), fontText), mTexts.end());
}

std::shared_ptr<Graphic> Game::getGraphic()
{
	return mGraphic;
}

void Game::input()
{

	if (GetAsyncKeyState('I')) {
		removeActor(mActors[3]);
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

	mGraphic->updatePointLight(mPointLights);
	mGraphic->updateSpotLight(mSpotLights);

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
	mGraphic->begin3DRender();
	mGraphic->setRenderType(Graphic::RENDER_3D);
	for (auto& mesh : mMeshes) {
		mesh->draw();
	}

	//2D描
	mGraphic->setRenderType(Graphic::RENDER_2D);
	for (auto& sprite : mSprites) {
		sprite->draw();
	}

	mGraphic->end3DRender();

	////テキスト描画
	mGraphic->begin2DRender();
	for (auto& text : mTexts) {
		text->draw();
	}

	mGraphic->end2DRender();

	mGraphic->moveToNextFrame();


}
