#include "Game.h"
Game::Game(){
	init();
}

Game::~Game() {}

void Game::runLoop()
{
	while (!mGraphic->quit()) {
		input();
		update();
		draw();
	}
}

int Game::endProcess()
{
	return mGraphic->msg_wparam();
}

void Game::init() {
	mGraphic = std::make_shared<Graphic>();
	mGraphic->init();
	mGraphic->clearColor(0.25f, 0.5f, 0.9f);

	//デバック用
	HRESULT hr;


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
	};

	const char* text[] = {
		"assets\\Alicia\\FBX\\Alicia.txt",
	};

#if 1
	//FBX→テキスト変換
	FBXConverter fbxConverter;
	const int fbxNum = _countof(fbx);
	int i;
	for (i = 0; i < fbxNum; i++) {
		fbxConverter.fbxToTxt(fbx[i], text[i], 0.1f, 0.1f, 0.1f);
	}
#endif

	//メッシュの生成
	const int num = _countof(text);
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(mGraphic);
	mMeshes.push_back(mesh);

	for (int i = 0; i < num; i++) {
		mMeshes[i]->create(text[i]);
	}

	//タイマー初期化
	initDeltaTime();


}

void Game::input()
{

}

void Game::update()
{
	setDeltaTime();

	time += 0.7f * delta;
	mMeshes[0]->ry(time);
	mMeshes[0]->update();
}

void Game::draw()
{
	//描画
	mGraphic->beginRender();
	mMeshes[0]->draw();
	mGraphic->endRender();
}
