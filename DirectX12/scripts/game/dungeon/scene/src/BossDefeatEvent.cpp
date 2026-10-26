#include "BossDefeatEvent.h"
#include "DungeonScene.h"
#include "EnemyComponent.h"
#include "EnemySpawner.h"
#include "Player.h"
#include "Object.h"
#include "Game.h"
#include "AudioManager.h"
#include "MapGenerator.h"
#include "timer.h"
#include <algorithm>

BossDefeatEvent::BossDefeatEvent(Dungeon2Scene& scene)
	: mScene(scene),
	mPhase(Phase::Idle),
	mBoss(nullptr),
	mDefeatSE(nullptr),
	mBossStartY(0.0f),
	mSinkSpeed(0.8f),				//TODO: SEの長さに合わせて調整
	mSinkDepth(MAPTIPSIZE * 3.0f),	//TODO: ボスのメッシュの高さに合わせて調整
	mGoalPathTiles{ { {3,6}, {3,7} } }
{
}

void BossDefeatEvent::start(EnemyComponent& boss)
{
	if (mPhase != Phase::Idle) return;
	mPhase = Phase::Defeated;

	//沈める対象のボスを保持
	mBoss = &boss.getOwner();
	mBossStartY = mBoss->getPosition().y;

	//ボス以外の敵を消し、積まれている召喚要求も破棄する
	despawnEnemies(boss);
	mScene.getEnemySpawner().clear();

	//以降APを消費しない。とどめの一撃で消費したAPは回復して相殺する
	Player* player = mScene.getPlayer();
	if (player) {
		player->setAPConsumption(false);
		player->recoverAP(1);
		mScene.updateAPUI();
	}

	//BGMを止めて撃破SEを流す
	AudioManager& audio = mScene.getGame().getAudioManager();
	audio.finishBGM();
	mDefeatSE = audio.playSE("BOSS_DEFEATED");
}

void BossDefeatEvent::update()
{
	if (mPhase != Phase::Defeated) return;

	//SE再生中は揺らし続ける(毎フレーム呼ぶことで最大強度を維持し、呼ばなくなると自然に減衰する)
	if (Player* player = mScene.getPlayer()) player->startCameraShake();

	//ボスを沈める
	sinkBoss();

	//SEが終わったらイベントの後半へ
	if (!mScene.getGame().getAudioManager().isPlaying(mDefeatSE)) {
		finish();
	}
}

void BossDefeatEvent::reset()
{
	mPhase = Phase::Idle;
	mBoss = nullptr;
	mDefeatSE = nullptr;
}

void BossDefeatEvent::despawnEnemies(const EnemyComponent& boss)
{
	//despawnは状態の変更のみで敵配列は変更しないため、走査中に呼んでも安全
	for (auto enemy : mScene.getEnemies()) {
		if (enemy == &boss) continue;
		enemy->despawn();
	}
}

void BossDefeatEvent::sinkBoss()
{
	if (!mBoss) return;

	XMFLOAT3 pos = mBoss->getPosition();
	pos.y = (std::max)(pos.y - mSinkSpeed * deltaTime, mBossStartY - mSinkDepth);
	mBoss->setPosition(pos);
}

void BossDefeatEvent::finish()
{
	mPhase = Phase::Finished;
	mDefeatSE = nullptr;

	//ボスを除去(EnemyComponentの終了処理でマップデータ・敵配列からも外れる)
	if (mBoss) {
		mBoss->setState(Actor::State::Dead);
		mBoss = nullptr;
	}

	//クリアBGMを流す
	mScene.getGame().getAudioManager().playBGM("BGM_CLEAR");

	//ゴールへの道を作り、ゴールを隠しているオブジェクトを消す
	openGoalPath();
	mScene.removeBossObject();
}

void BossDefeatEvent::openGoalPath()
{
	for (const auto& pos : mGoalPathTiles) {
		//マップデータを床に変更
		mScene.setTileDataAt(pos[0], pos[1], TileType::FLOOR);
		//床の生成
		auto floor = std::make_unique<Object>(mScene, "Tile", "ROCK_FLOOR", static_cast<float>(MAPTIPSIZE * pos[0]), static_cast<float>(MAPTIPSIZE * pos[1]));
		mScene.addActor(std::move(floor));
	}

	//ミニマップに道を反映
	mScene.updateMiniMapPos();
}
