#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include "stdafx.h"
#include "Buffer.h";
#include "BIN_FILE12.h"
#include "FBXConverter.h"
#include "Graphic.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

//頂点位置バッファ
ComPtr<ID3D12Resource> VertexBuf;
D3D12_VERTEX_BUFFER_VIEW VertexBufView;

ComPtr<ID3D12Resource> PositionBuf;
D3D12_VERTEX_BUFFER_VIEW PositionBufView;
//テクスチャ座標バッファ
ComPtr<ID3D12Resource> TexcoordBuf;
D3D12_VERTEX_BUFFER_VIEW TexcoordBufView;

//頂点インデックスバッファ
ComPtr<ID3D12Resource> IndexBuf;
D3D12_INDEX_BUFFER_VIEW IndexBufView;

//コンストバッファ0
struct CONST_BUF0 {
	XMMATRIX mat;
};
//コンスタバッファ1
struct CONST_BUF1 {
	XMFLOAT4 diffuse;
};
CONST_BUF0* MappedConstBuf0;
ComPtr<ID3D12Resource> ConstBuf0;
CONST_BUF1* MappedConstBuf1;
ComPtr<ID3D12Resource> ConstBuf1;
ComPtr<ID3D12Resource> TextureBuf;
