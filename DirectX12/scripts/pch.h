#pragma once
// プリコンパイル済みヘッダ
// 変更頻度が低い外部ヘッダ(標準ライブラリ / Windows / DirectX / json)のみを含める。
// プロジェクト内のヘッダはここに追加しないこと(変更のたびに全体が再ビルドされる)。
// プロジェクト設定で全ての .cpp に強制インクルードされる。

// ---- 標準ライブラリ ----
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// ---- Windows / DirectX ----
#include <windows.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <dxgi1_6.h>

// ---- サードパーティ ----
#include "json.hpp"
