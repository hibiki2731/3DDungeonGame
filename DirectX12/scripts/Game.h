#pragma once
#include "stdafx.h"
#include "Buffer.h"
#include "BIN_FILE12.h"
#include "FBXConverter.h"
#include "Graphic.h"
#include "Mesh.h"
#include "timer.h"
#include <vector>

using namespace DirectX;
using Microsoft::WRL::ComPtr;


class Game {
public:
	Game();
	~Game();
	//ループの開始
	void runLoop();
	//終了処理
	int endProcess();

private:
	//初期化
	void init();
	//ループ内処理
	void input();
	void update();
	void draw();


	std::shared_ptr<Graphic> mGraphic;

	std::vector<std::shared_ptr<Mesh>> mMeshes;

	UINT NumMeshes = 0;
	float yRot = 0;
	float xRot = 0;
};