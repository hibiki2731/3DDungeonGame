#pragma once
#include <DirectXMath.h>
using namespace DirectX;

const int MAX_LIGHT_NUM = 16;

enum LightStat {
    INACTIVE = 0,
    ACTIVE = 1,
};

struct PointLightData {
	XMFLOAT4 position; //xyz:位置、w:有効無効
    XMFLOAT4 color; //xyzw:rgba
    XMFLOAT4 setValue; //x:有効無効、y:光強度、z:光の届く範囲
};

struct SpotLightData {

    XMFLOAT4 position; //xyz:座標
    XMFLOAT4 direction; //xyz:向き
    XMFLOAT4 color; //xyz:rgb w:α値
    XMFLOAT4 setValue; //x:有効無効 y:光強度 z:光の届く距離
    XMFLOAT4 attAngle; //x:角度減衰が起こらない範囲 y:ライトがあたる範囲

};

//コンスタントバッファ０-----------------------------------------------------------------
struct CB0 {
    XMMATRIX viewProj;
    XMFLOAT4 cameraPos;
    PointLightData pointLights[MAX_LIGHT_NUM];
	SpotLightData spotLights[MAX_LIGHT_NUM];
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