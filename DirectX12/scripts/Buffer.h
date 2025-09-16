#pragma once
#include "stdafx.h"

using namespace DirectX;

struct Buffer {

};

//コンストバッファ0
struct CONST_BUF0 {
	XMMATRIX mat;
};
//コンスタバッファ1
struct CONST_BUF1 {
	XMFLOAT4 diffuse;
};
