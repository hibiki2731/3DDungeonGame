#include "Game.h"
Game::Game(){
	init();
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

	//メッシュの生成
	const int num = _countof(text);
	for (int i = 0; i < num; i++) {
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(mGraphic);
		mMeshes.push_back(mesh);
	}

	for (int i = 0; i < num; i++) {
		mMeshes[i]->create(text[i]);
	}
	//タイマー初期化
	initDeltaTime();


}

void Game::input()
{
	if (GetAsyncKeyState(VK_RIGHT)) {
		yRot += 0.7f * delta;
	}
	if (GetAsyncKeyState(VK_LEFT)) {
		yRot -= 0.7f * delta;
	}
	if (GetAsyncKeyState(VK_UP)) {
		xRot += 0.7f * delta;
	}
	if (GetAsyncKeyState(VK_DOWN)) {
		xRot -= 0.7f * delta;
	}
}

void Game::update()
{
	mMeshes[0]->px(-1.0f);
	mMeshes[1]->px(1.0f);
	mMeshes[0]->ry(yRot);
	mMeshes[1]->ry(yRot);
	mMeshes[0]->rx(xRot);
	mMeshes[1]->rx(xRot);
	for (UINT i = 0; i < mMeshes.size(); i++) {
		mMeshes[i]->update();
	}
}

void Game::draw()
{
	//描画
	mGraphic->beginRender();
	for (UINT i = 0; i < mMeshes.size(); i++) {
		mMeshes[i]->draw();
	}
	mGraphic->endRender();
}
