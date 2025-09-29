#pragma once
#include <DirectXMath.h>
#define PI 3.141592

using namespace DirectX;

XMFLOAT3 operator+(const XMFLOAT3& v1, const XMFLOAT3& v2);

XMFLOAT3 operator-(const XMFLOAT3& v1, const XMFLOAT3& v2);

XMFLOAT3 operator*(const XMFLOAT3& v1, const float& val);

XMFLOAT3 rotateY(const XMFLOAT3& v1, const float& rot);
XMFLOAT3 rotateX(const XMFLOAT3& v1, const float& rot);
XMFLOAT3 rotateZ(const XMFLOAT3& v1, const float& rot);