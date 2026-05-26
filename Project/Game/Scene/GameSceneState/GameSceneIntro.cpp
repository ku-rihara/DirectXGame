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
    alpha_           = 1.0f;
    shouldTransition_ = false;
    phase_           = [this](float t) { PhaseFadeIn(t); };
}

void GameSceneIntro::Update([[maybe_unused]] float timeSpeed) {
    if (KetaEngine::Frame::DeltaTime() >= 0.5f) {
        return;
    }

    auto& obj = pOwner_->GetGameObj();

    obj.screenSprite_->SetAlpha(alpha_);

    obj.gameIntroManager_->Update();
    obj.operateUI_->Update();

    phase_(timeSpeed);

    obj.attackEffect_->Update();
    obj.skyBox_->Update();
    obj.audienceController_->Update();
    obj.comboDirector_->Update();
    obj.backGroundObjectManager_->Update(KetaEngine::Frame::DeltaTime() * obj.gameIntroManager_->GetCurrentPlaySpeedRate());
    obj.gameCamera_->Update(obj.gameIntroManager_->GetCurrentPlaySpeedRate());
    obj.combo_->Update();

    ViewUpDate();

    if (shouldTransition_) {
        pOwner_->ChangeState(std::make_unique<GameScenePlaying>(pOwner_));
    }
}

void GameSceneIntro::PhaseFadeIn([[maybe_unused]] float timeSpeed) {
    alpha_ -= KetaEngine::Frame::DeltaTime();
    if (alpha_ <= 0.0f) {
        alpha_ = 0.0f;
        phase_ = [this](float t) { PhaseIntro(t); };
    }
}

void GameSceneIntro::PhaseIntro([[maybe_unused]] float timeSpeed) {
    auto& obj = pOwner_->GetGameObj();

    if (obj.gameIntroManager_->GetIsFinishStep(GameIntroManager::SpawnField)) {
        obj.enemyManager_->GetSpawner()->Update(KetaEngine::Frame::DeltaTimeRate());
        obj.enemyManager_->Update();
        obj.enemyManager_->UIUpdate(pOwner_->GetViewProjection());
    }

    if (obj.gameIntroManager_->GetIsFinishStep(GameIntroManager::AppearPurpose)) {
        shouldTransition_ = true;
    }
}

void GameSceneIntro::Debug() {
}