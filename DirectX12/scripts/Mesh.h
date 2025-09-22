#pragma once

//#define USE_INDEX
#include "stdafx.h"
#include "Graphic.h"
#include<map>
#include<memory>

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

class Mesh
{
public:
    Mesh(std::shared_ptr<Graphic> graphic);
    ~Mesh();
    void create(const char* filename);
    void update();
    void draw();
    //Setter
    void px(float px) { Px = px; }
    void py(float py) { Py = py; }
    void pz(float pz) { Pz = pz; }
    void rx(float rx) { Rx = rx; }
    void ry(float ry) { Ry = ry; }
    void rz(float rz) { Rz = rz; }
private:
    float Px, Py, Pz;
    float Rx, Ry, Rz;

    HRESULT Hr;

    //コマンドリスト graphic.cppから引っ張ってきます
	std::shared_ptr<Graphic> mGraphic;
    ComPtr<ID3D12GraphicsCommandList> CommandList;

    //コンスタントバッファ1(World Matrix)
    CB1* Cb1;
    ComPtr<ID3D12Resource> ConstBuf1;
    ComPtr<ID3D12DescriptorHeap> Cb1vHeap;//全パーツでシェアする

    //メッシュパーツ
    int NumParts;
    struct PARTS {
        //頂点バッファ
        UINT NumVertices;
        ComPtr<ID3D12Resource> VertexBuf;
        D3D12_VERTEX_BUFFER_VIEW VertexBufView;
        //コンスタントバッファ2(マテリアル)
        CB2* Cb2;//マップしたアドレスを入れる
        ComPtr<ID3D12Resource> ConstBuf2;
        //テクスチャバッファ
        ComPtr<ID3D12Resource> TextureBuf;
    }*Parts;

    //ディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> CbvTbvHeap;
    UINT CbvTbvSize;//ビューのサイズ
    UINT NumDescriptors;//ひとつのパーツで使用するディスクリプタの数
};