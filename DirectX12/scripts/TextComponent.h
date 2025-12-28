#pragma once
#include "Component.h"
#include "Graphic.h"
#include <string>

class TextComponent : public Component
{
public:
	TextComponent() {};
	~TextComponent();
	void initComponent() override;
	void draw();
	//テキストの表示、非表示
	void showText();
	void closeText();

	//セッター
	void setText(const std::wstring& text);
	void setBaseLine(float x, float y);
	void setFontSize(FLOAT size);
	void setTextColor(const D2D1::ColorF& color);

	//ゲッター
	bool getIsActive();

private:
	D2D1::ColorF mTextColor = D2D1::ColorF(0, 0, 0);
	std::shared_ptr<Graphic> mGraphic;
	ComPtr<ID2D1SolidColorBrush> mTextBrush;
	ComPtr<IDWriteTextFormat> mTextFormat;

	std::wstring mText;
	bool isActive;

	float mBaseLineX;
	float mBaseLineY;
	FLOAT mFontSize;
	const WCHAR* mFontName;
	D2D1_RECT_F mTextRect;

	int mMaxRow;
};

