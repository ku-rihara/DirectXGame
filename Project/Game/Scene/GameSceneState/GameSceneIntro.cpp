#include "GameSceneIntro.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "GameScenePlaying.h"
#include "Scene/GameScene.h"

GameSceneIntro::GameSceneIntro(GameScene* gameScene)
    : BaseGameSceneState("GameSceneIntro", gameScene) {
    Init();
}

void GameSceneIntro::Init() {
    alpha_         = 1.0f;
    isFirstChange_ = false;

  
}

void GameSceneIntro::Update([[maybe_unused]] float timeSpeed) {
    if (KetaEngine::Frame::DeltaTime() >= 2.0f) {
        return;
    }

    auto& obj = pOwner_->GetGameObj();

    // フェードイン処理
    obj.screenSprite_->SetAlpha(alpha_);

    if (!isFirstChange_) {
        alpha_ -= KetaEngine::Frame::DeltaTime();

        if (alpha_ <= 0.0f) {
            alpha_         = 0.0f;
            isFirstChange_ = true;
        }
    }

    // gameIntro
    obj.gameIntroManager_->Update();
    obj.howToOperate_->Update();

    if (obj.gameIntroManager_->GetIsFinishStep(GameIntroManager::SpawnField)) {
        obj.enemySpawner_->Update(KetaEngine::Frame::DeltaTimeRate());
        obj.enemyManager_->Update();
        obj.enemyManager_->HpBarUpdate(pOwner_->GetViewProjection());
    }

    // Editor
    obj.attackEffect_->Update();

    // obj
    obj.skyBox_->Update();
    obj.gameCamera_->Update(obj.gameIntroManager_->GetCurrentPlaySpeedRate());
    obj.combo_->Update();

    // イントロ完了でプレイ状態へ遷移
    if (obj.gameIntroManager_->GetIsFinishStep(GameIntroManager::AppearPurpose)) {
        pOwner_->ChangeState(std::make_unique<GameScenePlaying>(pOwner_));
    }
}

void GameSceneIntro::Debug() {
}