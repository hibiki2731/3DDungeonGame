#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cmath>
#include <cassert>
#include <fstream>
#include <DirectXMath.h>

#include <wrl/client.h>
using namespace DirectX;
using Microsoft::WRL::ComPtr;