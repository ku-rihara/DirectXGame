#include "GameSceneFinish.h"
#include "Scene/GameScene.h"
#include "Scene/Manager/SceneManager.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"

GameSceneFinish::GameSceneFinish(GameScene* gameScene)
    : BaseGameSceneState("GameSceneFinish", gameScene) {
    Init();
}

void GameSceneFinish::Init() {
    phase_ = Phase::kSpriteFallIn;
    alpha_ = 0.7f;

    // 暗転スプライトを即表示
    auto* screen = pOwner_->GetGameObj().screenSprite_.get();
    screen->SetIsDraw(true);
    screen->SetAlpha(alpha_);

    // 上から降ってくるスプライト
    clearSprite_.reset(KetaEngine::Sprite::Create("TimeUP.dds"));
    clearSprite_->PlaySpriteEaseAnimation("finishSpritePos", "GameSceneFinish");

    // フェードイン用イージング
    KetaEngine::EasingParameter<float> fadeParam;
    fadeParam.type       = EasingType::OutCubic;
    fadeParam.startValue = 0.7f;
    fadeParam.endValue   = 1.0f;
    fadeParam.maxTime    = 1.0f;
    fadeEasing_.SettingValue(fadeParam);
    fadeEasing_.SetAdaptValue(&alpha_);
}

void GameSceneFinish::Update([[maybe_unused]] float timeSpeed) {
    auto* screen = pOwner_->GetGameObj().screenSprite_.get();

    switch (phase_) {
    case Phase::kSpriteFallIn:
        // 降ってくるアニメーションが終わったらフェードインへ
        if (clearSprite_->GetSpriteEaseAnimationPlayer()->IsFinished()) {
            // アニメーション終了時の位置を transform_.pos に焼き込む
            // (IsFinished後は GetAnimationPosition が {0,0} を返すため、最後の位置を確定させる)
            Vector2 finalPos = clearSprite_->GetSpriteEaseAnimationPlayer()->GetCurrentPosition();
            clearSprite_->transform_.pos += finalPos;
            clearSprite_->StopSpriteEaseAnimation();
            phase_ = Phase::kFadeIn;
        }
        break;

    case Phase::kFadeIn:
        // イージングで alpha 0.7 → 1.0 に更新
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

void GameSceneFinish::Debug() {
}
