#include "TextComponent.h"
#include "Actor.h"
#include "Game.h"

TextComponent::~TextComponent()
{
    delete mFontName;
}

void TextComponent::initComponent()
{
	mGraphic = mOwner->getGame()->getGraphic();
	isActive = false;
	mBaseLineX = 0.0f;
	mBaseLineY = 0.0f;
	mFontSize = 32;
	mFontName = L"MS Pñæí©";
	mMaxRow = 20;
	mTextRect = D2D1::RectF(
		mBaseLineX,
		mBaseLineY,
		mMaxRow * mFontSize,
		mFontSize
	);
	mTextColor = D2D1::ColorF(D2D1::ColorF::White);
	mText = L"";

	mOwner->getGame()->addText(std::dynamic_pointer_cast<TextComponent>(shared_from_this()));
}

void TextComponent::draw()  
{  
   mGraphic->getD2DDeviceContext()->SetTransform(D2D1::Matrix3x2F::Identity());  
   mGraphic->getD2DDeviceContext()->DrawTextW(  
       mText.c_str(), 
       static_cast<UINT32>(mText.size() - 1), 
       mTextFormat.Get(),  
       &mTextRect,
       mTextBrush.Get()  
   );  
}

void TextComponent::endProccess()
{
	mOwner->getGame()->removeText(std::dynamic_pointer_cast<TextComponent>(shared_from_this()));
}

void TextComponent::showText()
{
	mTextRect = D2D1::RectF(
		mBaseLineX,
		mBaseLineY,
		mMaxRow * mFontSize,
		mFontSize * (mText.size() / mMaxRow + 1)
	);

    HRESULT hr = mGraphic->getD2DDeviceContext()->CreateSolidColorBrush(mTextColor, &mTextBrush);
	assert(SUCCEEDED(hr));
    hr = mGraphic->getDWriteFactory()->CreateTextFormat(
        mFontName,
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        mFontSize,
        L"en-us",
        &mTextFormat
    );
    assert(SUCCEEDED(hr));

    hr = mTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); //ç∂ëµÇ¶
	assert(SUCCEEDED(hr));

	hr = mTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR); //è„ëµÇ¶
	assert(SUCCEEDED(hr));

	isActive = true;
}

void TextComponent::closeText()
{
	isActive = false;
}

void TextComponent::setText(const std::wstring& text)
{
	mText = text;
}

void TextComponent::setBaseLine(float x, float y)
{
	mBaseLineX = x;
	mBaseLineY = y;
}

void TextComponent::setFontSize(FLOAT size)
{
	mFontSize = size;
}

void TextComponent::setTextColor(const D2D1::ColorF& color)
{
	mTextColor = color;
}

bool TextComponent::getIsActive()
{
	return isActive;
}
