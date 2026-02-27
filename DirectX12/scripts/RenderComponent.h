#pragma once
#include <memory>
#include <d3d12.h>
#include <DirectXMath.h>
#include "Component.h"
#include "Graphic.h"


class RenderComponent :
    public Component
{
public:
    virtual void create(const char* filename) {};
    virtual void draw() {};

    //ÉQÉbÉ^Å[
    Graphic* getGraphic() { return mGraphic; };
    ComPtr<ID3D12GraphicsCommandList> getCommandList() { return mCommandList; };

private:
    Graphic* mGraphic;
    ComPtr<ID3D12GraphicsCommandList> mCommandList;
};

