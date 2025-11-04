#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct Buffer {

};

enum LightStat {
    INACTIVE = 0,
    ACTIVE = 1,
};

struct Light {
    XMFLOAT4 position;
	XMFLOAT4 isActive;
};

//コンスタントバッファ０-----------------------------------------------------------------
struct CB0 {
    XMMATRIX viewProj;
    XMFLOAT4 cameraPos;
    Light lights[4];
    //std::vector<Light> lights;
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