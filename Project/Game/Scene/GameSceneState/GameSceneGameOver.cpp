#include "GameSceneGameOver.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"
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
    gameOverSprite_->PlaySpriteEaseAnimation("finishSpritePos", "GameSceneFinish");

    // フェードイン用イージング
    fadeEasing_.Init("GameSceneFadeIn.json");
    fadeEasing_.SetAdaptValue(&alpha_);
}

void GameSceneGameOver::Update([[maybe_unused]] float timeSpeed) {

    // HitStopタイマーを更新
    pOwner_->GetGameObj().attackEffect_->Update();

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
