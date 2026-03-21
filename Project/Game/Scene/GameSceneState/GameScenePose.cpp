#include "GameScenePose.h"
#include "GameScenePlaying.h"
#include "Scene/GameScene.h"

GameScenePose::GameScenePose(GameScene* gameScene)
    : BaseGameSceneState("GameScenePose", gameScene) {
    Init();
}

void GameScenePose::Init() {
    // TODO: 後程ポーズ用スプライト画像に差し替え
    poseSprite_.reset(KetaEngine::Sprite::Create("white1x1.dds"));
}

void GameScenePose::Update([[maybe_unused]] float timeSpeed) {
    // ポーズボタンで再びゲームに戻る
    if (pauseController_.IsPauseTriggered()) {
        pOwner_->ChangeState(std::make_unique<GameScenePlaying>(pOwner_));
    }
}

void GameScenePose::Debug() {
}
