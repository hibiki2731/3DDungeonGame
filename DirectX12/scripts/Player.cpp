#include "Player.h"
#include "Component.h"
#include "MeshComponent.h"

Player::~Player()
{
}

void Player::inputActor()
{
	XMFLOAT3 rot = getRotation();
	XMFLOAT3 updateRot = { 0, 0, 0 };

	if (GetAsyncKeyState(VK_RIGHT)) {
		updateRot.y += 0.7f * deltaTime;
	}
	if (GetAsyncKeyState(VK_LEFT)) {
		updateRot.y -= 0.7f * deltaTime;
	}
	if (GetAsyncKeyState(VK_UP)) {
		updateRot.x += 0.7f * deltaTime;
	}
	if (GetAsyncKeyState(VK_DOWN)) {
		updateRot.x -= 0.7f * deltaTime;
	}
	rot = rot + updateRot;
	setRotation(rot);
}

void Player::updateActor()
{
}

void Player::initActor()
{
	 auto mesh = createComponent<MeshComponent>(shared_from_this());
	 mesh->create("assets\\Alicia\\FBX\\Alicia.txt");
}

Player::Player()
{
}
