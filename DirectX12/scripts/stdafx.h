#pragma once

#include <iostream>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cmath>
#include <cassert>
#include <fstream>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <algorithm>

#include <wrl/client.h>
using namespace DirectX;
using Microsoft::WRL::ComPtr;

#include "timer.h"
#include "Buffer.h"
#include "input.h"
#include "Math.h"