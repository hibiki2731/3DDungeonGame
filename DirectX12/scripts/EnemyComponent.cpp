#include "EnemyComponent.h"
#include "Actor.h"
#include "Game.h"

void EnemyComponent::initComponent()
{
	CharacterComponent::initComponent();
	mOwner->getGame()->addEnemy(dynamic_pointer_cast<EnemyComponent>(shared_from_this()));

	mFlashTimer = 0.0f;
	mFlashDuration = 0.3f;	//ダメージを受けたときの点滅時間
}

void EnemyComponent::inputComponent()
{
	CharacterComponent::inputComponent();
}

void EnemyComponent::updateComponent()
{
	CharacterComponent::updateComponent();

	//インデックス座標の更新
	mIndexPos[0] = static_cast<int>(std::round(mOwner->getPosition().x / MAPTIPSIZE));
	mIndexPos[1] = static_cast<int>(std::round(mOwner->getPosition().z / MAPTIPSIZE));

	//点滅処理の更新
	updateFlash();

	//死亡したらActor配列から除去
	if (!isAlive) {
		mOwner->getGame()->removeActor(mOwner);
	}

}	

void EnemyComponent::endProccess()
{
	CharacterComponent::endProccess();
	mOwner->getGame()->removeEnemy(dynamic_pointer_cast<EnemyComponent>(shared_from_this()));
}

void EnemyComponent::setMesh(const std::shared_ptr<MeshComponent>& mesh)
{
	mMesh = mesh;
}

void EnemyComponent::startFlash()
{
	mFlashTimer = mFlashDuration;
}

XMFLOAT3 EnemyComponent::getPosition()
{
	return mOwner->getPosition();
}

void EnemyComponent::updateFlash()
{
	if (mMesh == nullptr) return;	//メッシュがセットされていない場合は処理しない

	//点滅処理
	if(mFlashTimer > 0.0f) {
		mFlashTimer -= deltaTime;
		float intensity = max(0.0f, mFlashTimer / mFlashDuration);
		mMesh->updateFlashIntensity(intensity * intensity);
	}
}
