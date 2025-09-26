#pragma once
#include <memory>
#include <d3d12.h>
#include <DirectXMath.h>
#include "Component.h"
#include "Graphic.h"

class SpriteComponent :
    public Component
{
public:
    SpriteComponent() {};
    ~SpriteComponent();

    void initComponent() override;
    void create(const char* filename);
    void draw();

	//描画範囲のセッター
    void setRect(const XMFLOAT4& rect);
private:
    //描画範囲
	XMFLOAT4 mRect;//xy:位置、zw:幅高さ

    //デバック用
    HRESULT Hr;

    //グラフィック
    std::shared_ptr<Graphic> mGraphic;
    //コマンドリスト
    ComPtr<ID3D12GraphicsCommandList> mCommandList;

    //コンスタントバッファ1(World Matrix)
    CB3* Cb3;
    ComPtr<ID3D12Resource> ConstBuf3;
    ComPtr<ID3D12DescriptorHeap> Cb1vHeap;//全パーツでシェアする

    //頂点バッファ
    ComPtr<ID3D12Resource> VertexBuf;
    D3D12_VERTEX_BUFFER_VIEW VertexBufView;
    ComPtr<ID3D12Resource> IndexBuf;
    D3D12_INDEX_BUFFER_VIEW IndexBufView;
    //テクスチャバッファ
    ComPtr<ID3D12Resource> TextureBuf;

    //ディスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> CbvTbvHeap;
    UINT CbvTbvSize;//ビューのサイズ
    UINT NumDescriptors;//ひとつのパーツで使用するディスクリプタの数
};

