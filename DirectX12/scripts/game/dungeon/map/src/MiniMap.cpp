#include "MiniMap.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "Graphic.h"
#include "Player.h"
#include "DungeonScene.h"
#include "MyMath.h"

constexpr int DisplayRange = 2;
constexpr int TileNum = (DisplayRange * 2 + 1) * (DisplayRange * 2 + 1);
constexpr XMFLOAT2 CanvasSize = { Graphic::ClientWidth * 0.15f, Graphic::ClientWidth * 0.15f};
constexpr XMFLOAT2 IconSize = { CanvasSize.x / (2 * DisplayRange + 1), CanvasSize.y / (2 * DisplayRange + 1) };

MiniMap::MiniMap(DungeonScene& scene) 
	: Actor(scene),
	 mDungeonScene(scene)
{
	//ミニマップの背景
	auto canvas = std::make_unique<SpriteComponent>(*this, 20.0f);
	canvas->create("assets/picture/minimap_black.png");
	canvas->setBordarSize(24.0f);
	canvas->setSpriteSize(CanvasSize);
	mMiniMapPosition = XMFLOAT3(Graphic::ClientWidth - CanvasSize.x, 0.0f, 20.0f);
	canvas->setPosition(mMiniMapPosition);
	addComponent(std::move(canvas));

	//ミニマップの外枠
	auto border = std::make_unique<SpriteComponent>(*this, 10.0f);
	border->create("assets/picture/UI2/PNG/Default/panel_border_grey_detail.png");
	border->setBordarSize(24.0f);
	border->setSpriteSize(CanvasSize * 1.03f);
	border->setPosition(mMiniMapPosition - XMFLOAT3(CanvasSize.x * 0.018f, CanvasSize.y * 0.002f, 10.0f));
	addComponent(std::move(border));

	//プレイヤーアイコン
	auto playerIcon = std::make_unique<SpriteComponent>(*this, 18.0f);
	//playerIcon->create("assets/picture/UI2/PNG/Default/minimap_arrow_c.png");
	playerIcon->create("assets/picture/minimap_player.png");
	playerIcon->setBordarSize(0.0f);
	playerIcon->setSpriteSize(IconSize);
	playerIcon->setPosition(mMiniMapPosition + XMFLOAT3(IconSize.x * 2.0f, IconSize.y * 2.0f, -2.0f));
	mPlayerIcon = playerIcon.get();
	addComponent(std::move(playerIcon));

	//タイルアイコン
	mTileIcon.resize(TileNum);
	for (int y = 0; y < DisplayRange * 2 + 1; y++){
		for (int x = 0; x < DisplayRange * 2 + 1; x++) {
			auto tileIcon = std::make_unique<SpriteComponent>(*this);
			tileIcon->create("assets/picture/minimap_tile.png");
			tileIcon->setBordarSize(0.0f);
			tileIcon->setSpriteSize(IconSize);
			tileIcon->setPosition(mMiniMapPosition + XMFLOAT3(IconSize.x * x, IconSize.y * y, 100.0f));
			//マップデータの座標と対応するよう、インデックスを調整
			mTileIcon[(DisplayRange * 2 + 1) * (DisplayRange * 2 - y) + x] = tileIcon.get();
			addComponent(std::move(tileIcon));
		}
	}

	//敵アイコン
	mEnemyIcon.resize(TileNum);
	for (int y = 0; y < DisplayRange * 2 + 1; y++) {
		for (int x = 0; x < DisplayRange * 2 + 1; x++) {
			auto enemyIcon = std::make_unique<SpriteComponent>(*this);
			//enemyIcon->create("assets/picture/UI2/PNG/Default/minimap_icon_jewel_red.png");
			enemyIcon->create("assets/picture/minimap_enemy.png");
			enemyIcon->setBordarSize(0.0f);
			enemyIcon->setSpriteSize(IconSize);
			enemyIcon->setPosition(mMiniMapPosition + XMFLOAT3(IconSize.x * x, IconSize.y * y, 100.0f));
			mEnemyIcon[(DisplayRange * 2 + 1) * (DisplayRange * 2 - y) + x] = enemyIcon.get();
			addComponent(std::move(enemyIcon));
		}
	}

	//リソースアイコン
	mResourceIcon.resize(TileNum);
	for (int y = 0; y < DisplayRange * 2 + 1; y++){
		for (int x = 0; x < DisplayRange * 2 + 1; x++) {
			auto resourceIcon = std::make_unique<SpriteComponent>(*this, 19.5f);
			//resourceIcon->create("assets/picture/UI2/PNG/Default/minimap_icon_star_yellow.png");
			resourceIcon->create("assets/picture/minimap_resource.png");
			resourceIcon->setBordarSize(0.0f);
			resourceIcon->setSpriteSize(IconSize);
			resourceIcon->setPosition(mMiniMapPosition + XMFLOAT3(IconSize.x * x, IconSize.y * y, -101.f));
			mResourceIcon[(DisplayRange * 2 + 1) * (DisplayRange * 2 - y) + x] = resourceIcon.get();
			addComponent(std::move(resourceIcon));
		}
	}
	
	//宝箱アイコン
	auto treasureIcon = std::make_unique<SpriteComponent>(*this, 19.5f);
	treasureIcon->create("assets/picture/minimap_chest.png");
	treasureIcon->setBordarSize(0.0f);
	treasureIcon->setSpriteSize(IconSize);
	treasureIcon->setPosition(mMiniMapPosition + XMFLOAT3(0.0f, 0.0f, -101.f));
	mTreasureIcon = treasureIcon.get();
	addComponent(std::move(treasureIcon));

	//ゴールアイコン
	auto goalIcon = std::make_unique<SpriteComponent>(*this, 19.5f);
	goalIcon->create("assets/picture/minimap_flag.png");
	goalIcon->setBordarSize(0.0f);
	goalIcon->setSpriteSize(IconSize);
	goalIcon->setPosition(mMiniMapPosition + XMFLOAT3(0.0f, 0.0f, -101.f));
	mGoalIcon = goalIcon.get();
	addComponent(std::move(goalIcon));
	

}

void MiniMap::updatePosition()
{
	const std::array<int, 2> playerPos = mDungeonScene.getPlayer()->getIndexPos();
	//宝箱、ゴールのアイコンの位置を初期化(隠す)
	mTreasureIcon->setPosZ(100.0f);
	mGoalIcon->setPosZ(100.0f);

	//各マスのマップデータを読み込む
	for (int i = 0; i < TileNum; i++) {
		int x = playerPos[0] + (i % (DisplayRange * 2 + 1)) - DisplayRange;
		int y = playerPos[1] + (i / (DisplayRange * 2 + 1)) - DisplayRange;

		//マップの範囲外
		if (x < 0 || x >= mDungeonScene.getMapSize()) {
			mTileIcon[i]->setPosZ(100.0f);
			mEnemyIcon[i]->setPosZ(100.0f);
			mResourceIcon[i]->setPosZ(100.0f);
			continue;
		}
		if (y < 0 || y >= mDungeonScene.getMapSize()) {
			mTileIcon[i]->setPosZ(100.0f);
			mEnemyIcon[i]->setPosZ(100.0f);
			mResourceIcon[i]->setPosZ(100.0f);
			continue;
		}

		//タイル情報
		int tileType = mDungeonScene.getTileDataAt(x, y);
		mTileIcon[i]->setPosZ(19.0f); //床は基本表示（壁や穴のみ非表示）
		switch (tileType) {
		case TileType::FLOOR:
		case TileType::RESOURCE:
			break;
		case TileType::TREASURE:
			mTreasureIcon->setPosition(XMFLOAT3(mMiniMapPosition + XMFLOAT3(IconSize.x * (i % (DisplayRange * 2 + 1)), IconSize.y * (DisplayRange * 2 - i / (DisplayRange * 2 + 1)), -2.0f)));
			break;
		case TileType::GOAL:
			mGoalIcon->setPosition(XMFLOAT3(mMiniMapPosition + XMFLOAT3(IconSize.x * (i % (DisplayRange * 2 + 1)), IconSize.y * (DisplayRange * 2 - i / (DisplayRange * 2 + 1)), -2.0f)));
			break;
		case TileType::EMPTY:
		case TileType::WALL:
		default:
			mTileIcon[i]->setPosZ(100.0f);	//非表示
			break;
		}

		//敵情報
		int objectType = mDungeonScene.getCharacterDataAt(x, y);
		if (objectType == CharacterType::ENEMY) mEnemyIcon[i]->setPosZ(18.0f);
		else mEnemyIcon[i]->setPosZ(100.0f);

		//リソース情報
		if (tileType == TileType::RESOURCE) mResourceIcon[i]->setPosZ(18.0f);
		else mResourceIcon[i]->setPosZ(100.0f);
	}
}

void MiniMap::updateDirection()
{
	int	direction = mDungeonScene.getPlayer()->getDirection();
	switch (direction) {
	case Direction::UP:
		mPlayerIcon->setRotation(0.0f);
		break;
	case Direction::DOWN:
		mPlayerIcon->setRotation(XM_PI);
		break;
	case Direction::RIGHT:
		mPlayerIcon->setRotation(XM_PIDIV2);
		break;
	case Direction::LEFT:
		mPlayerIcon->setRotation(-XM_PIDIV2);
		break;
	}

}
