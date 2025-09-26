#pragma once
#pragma once

//#define USE_INDEX
#include "stdafx.h"
#include "Graphic.h"
#include "Component.h"
#include<map>
#include<memory>

class MeshComponent : public Component
{
public:
    MeshComponent() {};
    ~MeshComponent();

    void initComponent() override;
    void create(const char* filename);
    void draw();
private:

    HRESULT Hr;

    //読み込みファイルネーム
	const char* mFilename;

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