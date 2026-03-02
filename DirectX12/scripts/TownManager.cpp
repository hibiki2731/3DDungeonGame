#include "TownManager.h"
#include "TextComponent.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Graphic.h"
#include "SceneManager.h"
#include "json.hpp"
#include "MyUtility.h"
#include "input.h"

TextWindow::TextWindow(Game* game, std::string windowName, int updateOrder) : Actor(game)
{
	mSelectedIndex = 0;
	mMaxIndex = 5;

	//jsonファイルからテキストウィンドウのパラメータを読み込む
	nlohmann::json textWindowData;
	std::ifstream file("assets\\data\\townMenuData.json");
	assert(!file.fail());
	file >> textWindowData;

	//ウィンドウの背景
	auto window = std::make_unique<SpriteComponent>(this, updateOrder);
	window->create(textWindowData[windowName]["spriteFileName"].get<std::string>());
	window->setBordarSize(textWindowData[windowName].value("borderSize", 24.0f));
	window->setSpriteSize(XMFLOAT2(textWindowData[windowName]["width"].get<float>(), textWindowData[windowName]["height"].get<float>()));
	XMFLOAT2 pos = XMFLOAT2(textWindowData[windowName]["x"].get<float>(), textWindowData[windowName]["y"].get<float>());
	window->setPosition(pos);
	addComponent(std::move(window));

	//テキスト
	std::wstring message = Utility::stringToWString(textWindowData[windowName]["text"].get<std::string>());
	float fontSize = textWindowData[windowName]["fontSize"].get<float>();
	float lineSpace = textWindowData[windowName].value("lineSpace", 0.0f);
	auto text = std::make_unique<TextComponent>(this);
	text->setText(message);
	text->setBaseLine(pos.x + textWindowData[windowName]["textOffsetX"].get<float>(), pos.y + textWindowData[windowName]["textOffsetX"].get<float>());
	text->setFontSize(fontSize);
	text->setTextColor(D2D1::ColorF(D2D1::ColorF::Black));
	text->setLineSpace(fontSize + lineSpace);
	text->showText();
	addComponent(std::move(text));

	//矢印
	auto arrow = std::make_unique<SpriteComponent>(this, updateOrder - 1);
	arrow->create(textWindowData[windowName]["arrowFileName"].get<std::string>());
	arrow->setBordarSize(0.0f);
	arrow->setSpriteSize(XMFLOAT2(textWindowData[windowName]["arrowWidth"].get<float>(), textWindowData[windowName]["arrowHeight"].get<float>()));
	arrow->setPosition(XMFLOAT2(pos.x + textWindowData[windowName]["arrowOffsetX"].get<float>(), pos.y + textWindowData[windowName]["arrowOffsetY"].get<float>()));
	arrow->setRotation(-XM_PIDIV2);
	mArrow = arrow.get();
	addComponent(std::move(arrow));

	mArrowMoveLength = fontSize + lineSpace;


}

void TextWindow::inputActor() {

	if (isKeyJustPressed(VK_UP)) {
		if (mSelectedIndex == 0) return;
		mSelectedIndex--;
		mArrow->movePositon(XMFLOAT2(0.0f, -mArrowMoveLength));
	}

	if (isKeyJustPressed(VK_DOWN)) {
		if (mSelectedIndex == mMaxIndex - 1) return;
		mSelectedIndex++;
		mArrow->movePositon(XMFLOAT2(0.0f, mArrowMoveLength));
	}
}

void TextWindow::setMaxIndex(int maxIndex)
{
	mMaxIndex = maxIndex;
}

BackGround::BackGround(Game* game) : Actor(game)
{
	auto window = std::make_unique<SpriteComponent>(this);
	window->create("assets\\picture\\town.png");
	window->setBordarSize(0.0f);
	window->setSpriteSize(XMFLOAT2(game->getGraphic()->getClientWidth(), game->getGraphic()->getClientHeight()));
	addComponent(std::move(window));
}

TownManager::TownManager(Game* game)
{
	mGame = game;
	isTown = false;
}

void TownManager::update()
{
	if (!isTown && mGame->getSceneManager()->getCurrentScene() == SceneType::TOWN) {
		isTown = true;
		auto bg = std::make_unique<BackGround>(mGame);
		mGame->addActor(std::move(bg));

		auto textWindow = std::make_unique<TextWindow>(mGame, "MainMenu", 99);
		mGame->addActor(std::move(textWindow));
	}

	else if (mGame->getSceneManager()->getCurrentScene() != SceneType::TOWN) {
		isTown = false;
	}
}
