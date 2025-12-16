#include "Math.h"

XMFLOAT3 normalZ = {0 ,0, 1};

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

XMFLOAT3 Math::rotateY(const XMFLOAT3& v1, const float& rot)
{
	return XMFLOAT3(v1.x * cosf(rot) - v1.z * sinf(rot), v1.y, v1.x * sinf(rot) + v1.z * cosf(rot));
}

XMFLOAT3 Math::rotateX(const XMFLOAT3& v1, const float& rot)
{
	return XMFLOAT3(v1.x, v1.y * cosf(rot) - v1.z * sinf(rot), v1.y * sinf(rot) + v1.z * cosf(rot));
}

XMFLOAT3 Math::rotateZ(const XMFLOAT3& v1, const float& rot)
{
	return XMFLOAT3(v1.x * cosf(rot) - v1.y * sinf(rot), v1.x * sinf(rot) + v1.y * cosf(rot),  v1.z);
}

FbxVector4 Math::translate(const FbxVector4& v, const FbxVector4& translation)
{
	return FbxVector4(v[0] + translation[0], v[1] + translation[1], v[2] + translation[2], v[3]);
}

FbxVector4 Math::rotateX(const FbxVector4& v1, const FbxDouble& rot)
{
	return FbxVector4(v1[0], v1[1] * cos(rot) - v1[2] * sin(rot), v1[1] * sin(rot) + v1[2] * cos(rot), v1[3]);
}

FbxVector4 Math::rotateY(const FbxVector4& v1, const FbxDouble& rot)
{
	return FbxVector4(v1[0] * cos(rot) - v1[2] * sin(rot), v1[1], v1[0] * sin(rot) + v1[2] * cos(rot), v1[3]);
}

FbxVector4 Math::rotateZ(const FbxVector4& v1, const FbxDouble& rot)
{
	return FbxVector4(v1[0] * cos(rot) - v1[1] * sin(rot), v1[0] * sin(rot) + v1[1] * cos(rot), v1[2], v1[3]);
}

FbxVector4 Math::scale(const FbxVector4& v, const FbxVector4& scale)
{
	return FbxVector4(v[0] * scale[0], v[1] * scale[1], v[2] * scale[2], v[3]);
}

FbxVector4 Math::normalize(const FbxVector4& v)
{
	double length = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	return FbxVector4(v[0] / length, v[1] / length, v[2] / length, 1.0);
}
