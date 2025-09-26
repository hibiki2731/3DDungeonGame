#pragma once
#include "stdafx.h"

using namespace DirectX;

struct Buffer {

};
//コンスタントバッファ０-----------------------------------------------------------------
struct CB0 {
    XMMATRIX viewProj;
    XMFLOAT4 lightPos;
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