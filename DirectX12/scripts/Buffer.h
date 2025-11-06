#pragma once
#include <DirectXMath.h>
using namespace DirectX;

const int MAX_LIGHT_NUM = 16;

enum LightStat {
    INACTIVE = 0,
    ACTIVE = 1,
};

struct Light {
	XMFLOAT4 position; //xyz:位置、w:有効無効
};

//コンスタントバッファ０-----------------------------------------------------------------
struct CB0 {
    XMMATRIX viewProj;
    XMFLOAT4 cameraPos;
	XMFLOAT4 pointLightsPos[MAX_LIGHT_NUM]; //xyz:位置、w:有効無効
};

//１つのメッシュに１つのCB1
struct CB1 {
    XMMATRIX world;
};

//１つのメッシュに複数のCB2
struct CB2 {
    XMFLOAT4 ambient;
    XMFLOAT4 diffuse;

    XMFLOAT4 specular;
};

//2D用バッファ
struct CB3 {
	XMMATRIX world;
	XMFLOAT4 rect; //xy:位置、zw:幅高さ
};