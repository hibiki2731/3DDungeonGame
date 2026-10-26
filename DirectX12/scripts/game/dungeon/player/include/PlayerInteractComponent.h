#pragma once
#include "Component.h"
#include <array>

class DungeonScene;
class Player;
class CharacterComponent;

//フィールド上の物に対する行動(資源の回収、宝箱、階段)を行うコンポーネント
class PlayerInteractComponent : public Component
{
public:
	PlayerInteractComponent(DungeonScene& scene, Player& player, CharacterComponent& character);
	DECLARE_COMPONENT_NAME(PlayerInteractComponent)

	//足元の資源を回収する。回収した場合はtrue(即時に終わる行動のため、そのまま行動終了する)
	bool collect();
	//前方の宝箱を開ける。開けた場合はtrue(即時に終わる行動のため、そのまま行動終了する)
	bool openTreasure();
	//前方に階段があるか判定
	bool downStairs();
	//前方にゴールがあるか判定
	bool isFacingGoal();

private:
	//プレイヤーの前方のインデックス座標を取得
	std::array<int, 2> getFrontIndexPos();

	DungeonScene& mScene;
	Player& mPlayer;
	CharacterComponent& mCharacter;
};
