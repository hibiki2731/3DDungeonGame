#pragma once
#include "Component.h"
#include "Definition.h"

class DungeonScene;
class CharacterComponent;

//プレイヤーの視点移動(90度回転)を行うコンポーネント
//回転の進行はPlayerRequestActionStateがupdateRotateを呼び出して行う
class PlayerRotateComponent : public Component
{
public:
	PlayerRotateComponent(DungeonScene& scene, Actor& owner, CharacterComponent& character, float rotSpeed);
	DECLARE_COMPONENT_NAME(PlayerRotateComponent)

	//回転を開始する(向きの更新はこの時点で行う)
	void startRotate(Direction direction);
	//回転を進める。回転が完了したらtrueを返す
	bool updateRotate();

private:
	DungeonScene& mScene;
	CharacterComponent& mCharacter;
	float mRotSpeed;		//回転速度
	XMFLOAT3 mTargetRot;	//回転先の回転角
};
