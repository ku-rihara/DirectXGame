#include "GameSceneGameOver.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"
#include "Easing/Easing.h"
#include "Frame/Frame.h"
#include "Scene/GameScene.h"
#include "Scene/Manager/SceneManager.h"

GameSceneGameOver::GameSceneGameOver(GameScene* gameScene)
    : BaseGameSceneState("GameSceneGameOver", gameScene) {
    Init();
}

void GameSceneGameOver::Init() {
    phase_ = Phase::kSpriteAnim;
    alpha_ = 0.7f;

    // TimeUpスプライト生成・アニメーション再生
    gameOverSprite_.reset(KetaEngine::Sprite::Create("TimeUp.dds"));
    gameOverSprite_->PlaySpriteEaseAnimation("GameOverAppear", "GameOver");

    // フェードイン用イージング（0.7 → 1.0）
    KetaEngine::EasingParameter<float> fadeParam;
    fadeParam.type       = EasingType::OutCubic;
    fadeParam.startValue = 0.7f;
    fadeParam.endValue   = 1.0f;
    fadeParam.maxTime    = 1.0f;
    fadeEasing_.SettingValue(fadeParam);
    fadeEasing_.SetAdaptValue(&alpha_);
}

void GameSceneGameOver::Update([[maybe_unused]] float timeSpeed) {
    auto* screen = pOwner_->GetGameObj().screenSprite_.get();

    switch (phase_) {
    case Phase::kSpriteAnim:
        if (gameOverSprite_->GetSpriteEaseAnimationPlayer()->IsFinished()) {
            Vector2 finalPos = gameOverSprite_->GetSpriteEaseAnimationPlayer()->GetCurrentPosition();
            gameOverSprite_->transform_.pos += finalPos;
            gameOverSprite_->StopSpriteEaseAnimation();

            screen->SetIsDraw(true);
            screen->SetAlpha(alpha_);
            phase_ = Phase::kFadeIn;
        }
        break;

    case Phase::kFadeIn:
        fadeEasing_.Update(timeSpeed);
        screen->SetAlpha(alpha_);

        if (fadeEasing_.IsFinished()) {
            KetaEngine::SceneManager::GetInstance()->ChangeScene("RESULT");
            return;
        }
        break;

    case Phase::kDone:
        break;
    }

    ViewUpDate();
}

void GameSceneGameOver::Debug() {
}
