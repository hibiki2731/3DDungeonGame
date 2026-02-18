#pragma once
#include "SpriteComponent.h"

class Anime2DComponent :
    public SpriteComponent
{
public:
	Anime2DComponent() {};
	~Anime2DComponent() {};
	void initComponent() override;
	virtual void create(const std::string filename, int textureNum);
	virtual void draw() override;

	void endProccess() override;


	void setTextureIndex(int index);

	int getTextureNum() { return mTextureNum; };

private:
	int mTextureIndex;
	int mTextureNum;

	std::vector<ComPtr<ID3D12Resource>> TextureBufs;

};

