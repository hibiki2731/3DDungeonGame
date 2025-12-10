#pragma once
#include "Component.h"
#include "Graphic.h"
#include <string>
#include <vector>
class CharFont {
public:
	CharFont();
	~CharFont();

	void init(std::shared_ptr<Graphic>& graphic, wchar_t* c, UINT fontSize = 64);

	//ゲッター
	ComPtr<ID3D12Resource>& getFontTexture();
	ComPtr<ID3D12Resource>& getConstBuf3();
	D3D12_VERTEX_BUFFER_VIEW& getVertexBufView();
	D3D12_INDEX_BUFFER_VIEW& getIndexBufView();
	GLYPHMETRICS& getmGm();

	//セッター
	void setOrigin(float x, float y);

private:
	void createPolygon();
	void createTexture(GLYPHMETRICS& gm, BYTE*& FontBitmap);

	UINT mFontSize;
	const WCHAR* mFontName =  L"MS UI Gothic" ;
	GLYPHMETRICS mGm;

	
	ComPtr<ID3D12Resource> mFontTexture;
	SpriteConstBuf* Cb3;
	ComPtr<ID3D12Resource> ConstBuf3;
	std::shared_ptr<Graphic> mGraphic;
	ComPtr<ID3D12Resource> VertexBuf;
	ComPtr<ID3D12Resource> IndexBuf;
	D3D12_VERTEX_BUFFER_VIEW VertexBufView;
	D3D12_INDEX_BUFFER_VIEW IndexBufView;
};

class FontTextComponent : public Component
{
public:
	FontTextComponent() {};
	~FontTextComponent() {};
	void initComponent() override;

	//描画
	void draw();

	//テキストの表示、非表示
	void showText();
	void closeText();

	//セッター
	void setText(const std::wstring& text);
	void setBaseLine(float x, float y);
	void setFontSize(UINT size);

	//ゲッター
	bool getIsActive();

private:
	bool isActive;

	float baseLineX;
	float baseLineY;
	UINT fontSize;
	int maxRow;
	std::vector<std::shared_ptr<CharFont>> mCharFonts;
	std::shared_ptr<Graphic> mGraphic;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;
	ComPtr<ID3D12DescriptorHeap> mCbvTbvHeap;
};

