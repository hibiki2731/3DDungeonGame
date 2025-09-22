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
//コンスタバッファ1
struct CONST_BUF1 {
	XMFLOAT4 diffuse;
};
