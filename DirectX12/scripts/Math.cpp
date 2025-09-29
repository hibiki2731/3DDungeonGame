#include "Math.h"

XMFLOAT3 operator+(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
    return XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
XMFLOAT3 operator-(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	return XMFLOAT3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

XMFLOAT3 operator*(const XMFLOAT3& v1, const float& val) {
	return XMFLOAT3(v1.x * val, v1.y * val, v1.z * val);
}

XMFLOAT3 rotateY(const XMFLOAT3& v1, const float& rot)
{
	return XMFLOAT3(v1.x * cosf(rot) - v1.z * sinf(rot), v1.y, v1.x * sinf(rot) + v1.z * cosf(rot));
}

XMFLOAT3 rotateX(const XMFLOAT3& v1, const float& rot)
{
	return XMFLOAT3(v1.x, v1.y * cosf(rot) - v1.z * sinf(rot), v1.y * sinf(rot) + v1.z * cosf(rot));
}

XMFLOAT3 rotateZ(const XMFLOAT3& v1, const float& rot)
{
	return XMFLOAT3(v1.x * cosf(rot) - v1.y * sinf(rot), v1.x * sinf(rot) + v1.y * cosf(rot),  v1.z);
}
