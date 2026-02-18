#pragma once
#pragma once

//#define USE_INDEX
#include <memory>
#include <d3d12.h>
#include <DirectXMath.h>
#include "Graphic.h"
#include "AssetManager.h"
#include "Component.h"

class MeshComponent : public Component
{
public:
    MeshComponent() {};
    ~MeshComponent();

    void initComponent() override;
	void endProccess() override;

    void create(ObjectName objectName);
	void draw();

    void updateFlashIntensity(float intensity);

private:

    HRESULT Hr;

    //コマンドリスト graphic.cppから引っ張ってきます
    std::shared_ptr<Graphic> mGraphic;
    ComPtr<ID3D12GraphicsCommandList> mCommandList;

    //コンスタントバッファ1(World Matrix)
    World3DConstBuf Cb1;

    //メッシュパーツ
    int NumParts;
    struct PARTS {
        //頂点バッファ
        UINT NumVertices;
        D3D12_VERTEX_BUFFER_VIEW VertexBufView;
        //コンスタントバッファ2(マテリアル)
        MaterialConstBuf Cb2;//マップしたアドレスを入れる
        //テクスチャバッファ
        ComPtr<ID3D12Resource> TextureBuf;
    }*Parts;

    //ディスクリプタヒープ
    UINT CbvTbvSize;//ビューのサイズ
    UINT NumDescriptors;//ひとつのパーツで使用するディスクリプタの数

    //使用するディスクリプタヒープおよびコンスタントバッファのインデックス
    int mHeapIndex;
    int mHeapSize;
    int mCBIndex;
    int mCBSize;
};