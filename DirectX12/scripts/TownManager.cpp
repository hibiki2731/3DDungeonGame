#include "TownManager.h"
#include "TextComponent.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "Graphic.h"
#include "SceneManager.h"
#include "json.hpp"
#include "MyUtility.h"
#include "input.h"

TextWindow::TextWindow(Game* game, std::string windowName, float zDepth) : Actor(game)
{
	mSelectedIndex = 0;
	mMaxIndex = 5;
	isActive = true;

	//jsonファイルからテキストウィンドウのパラメータを読み込む
	nlohmann::json textWindowData;
	std::ifstream file("assets\\data\\townMenuData.json");
	assert(!file.fail());
	file >> textWindowData;

	////ウィンドウの背景
	auto window = std::make_unique<SpriteComponent>(this, zDepth);
	window->create(textWindowData[windowName]["spriteFileName"].get<std::string>());
	window->setBordarSize(textWindowData[windowName].value("borderSize", 24.0f));
	window->setSpriteSize(XMFLOAT2(textWindowData[windowName]["width"].get<float>(), textWindowData[windowName]["height"].get<float>()));
	XMFLOAT3 pos = XMFLOAT3(textWindowData[windowName]["x"].get<float>(), textWindowData[windowName]["y"].get<float>(), zDepth);
	window->setPosition(pos);
	addComponent(std::move(window));

	//テキスト
	std::wstring message = Utility::stringToWString(textWindowData[windowName]["text"].get<std::string>());
	float fontSize = textWindowData[windowName]["fontSize"].get<float>();
	float lineSpace = textWindowData[windowName].value("lineSpace", 0.0f);
	auto text = std::make_unique<TextComponent>(this, zDepth - 0.5f);
	text->setText(message);
	text->setBaseLine(pos.x + textWindowData[windowName]["textOffsetX"].get<float>(), pos.y + textWindowData[windowName]["textOffsetX"].get<float>());
	text->setFontSize(fontSize);
	text->setTextColor(D2D1::ColorF(D2D1::ColorF::Black));
	text->setLineSpace(fontSize + lineSpace);
	text->showText();
	addComponent(std::move(text));

	//矢印
	auto arrow = std::make_unique<SpriteComponent>(this);
	arrow->create(textWindowData[windowName]["arrowFileName"].get<std::string>());
	arrow->setBordarSize(0.0f);
	arrow->setSpriteSize(XMFLOAT2(textWindowData[windowName]["arrowWidth"].get<float>(), textWindowData[windowName]["arrowHeight"].get<float>()));
	arrow->setPosition(XMFLOAT3(pos.x + textWindowData[windowName]["arrowOffsetX"].get<float>(), pos.y + textWindowData[windowName]["arrowOffsetY"].get<float>(), zDepth - 1.0f));
	arrow->setRotation(-XM_PIDIV2);
	mArrow = arrow.get();
	addComponent(std::move(arrow));

	mArrowMoveLength = fontSize + lineSpace;


}

void TextWindow::inputActor() {

	if (!isActive) return;

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

	if (isKeyJustPressed(VK_RETURN)) {
	}
}

int TextWindow::getSelectedIndex()
{
	return mSelectedIndex;
}
bool TextWindow::getIsActive()
{
	return isActive;
}
void TextWindow::setMaxIndex(int maxIndex)
{
	mMaxIndex = maxIndex;
}

void TextWindow::setActive(bool active)
{
	isActive = active;
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
	mMainMenu = nullptr;
	mSubMenu = nullptr;
}

void TownManager::update()
{
	//シーンがTONWに切り替わった際の処理
	if (!isTown && mGame->getSceneManager()->getCurrentScene() == SceneType::TOWN) {
		isTown = true;

		auto bg = std::make_unique<BackGround>(mGame);
		mGame->addActor(std::move(bg));

		auto textWindow = std::make_unique<TextWindow>(mGame, "MainMenu", 99.0f);
		mMainMenu = textWindow.get();
		mGame->addActor(std::move(textWindow));
	}

	//シーンがTONWNの時の処理
	if (isTown) {

		if (mMainMenu->getIsSelected()) {
			//メインメニューを非アクティブにする
			mMainMenu->resetSelected();
			mMainMenu->setActive(false);

			//選択されているメニューのインデックスに応じた処理
			switch (mMainMenu->getSelectedIndex()) {
			case 0: {
				//宿屋
				auto inn = std::make_unique<TextWindow>(mGame, "InnMenu", 97.0f);
				inn->setMaxIndex(2);
				mSubMenu = inn.get();
				mGame->addActor(std::move(inn));
				break;
			}
			case 1: {
				//道具屋
				auto shop = std::make_unique<TextWindow>(mGame, "ShopMenu", 97.0f);
				shop->setMaxIndex(2);
				mSubMenu = shop.get();
				mGame->addActor(std::move(shop));
				break;
			}
			case 2: {
				//鍛冶屋
				auto forge = std::make_unique<TextWindow>(mGame, "ForgeMenu", 97.0f);
				forge->setMaxIndex(2);
				mSubMenu = forge.get();
				mGame->addActor(std::move(forge));
				break;
			}
			case 3: {
				//探索道具屋
				auto explorerShop = std::make_unique<TextWindow>(mGame, "ExplorerShopMenu", 97.0f);
				explorerShop->setMaxIndex(2);
				mSubMenu = explorerShop.get();
				mGame->addActor(std::move(explorerShop));
				break;
			}
			case 4:
				//ダンジョンへ
				mGame->getSceneManager()->transitToMap();
				break;
			}
		}
	}

	//シーンがTONWN以外に切り替わった際の処理
	if (mGame->getSceneManager()->getCurrentScene() != SceneType::TOWN) {
		isTown = false;
		mMainMenu = nullptr;
	}
}

void TownManager::input()
{
	if (!isTown) return;

	if (mSubMenu != nullptr) {
		//メインメニュー以外のウィンドウがアクティブな時は、エスケープキーでメインメニューをアクティブにする
		if (isKeyJustPressed(VK_ESCAPE)) {
			mMainMenu->setActive(true);
			mSubMenu->setState(Actor::Dead);
			mSubMenu = nullptr;
		}
	}
}
