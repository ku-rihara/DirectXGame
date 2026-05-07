#include "GameSceneFinish.h"
#include "Scene/GameScene.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"

GameSceneFinish::GameSceneFinish(GameScene* gameScene, std::function<void()> onFinished)
    : BaseGameSceneState("GameSceneFinish", gameScene)
    , onFinished_(std::move(onFinished)) {
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
    fadeEasing_.Init("GameSceneFadeIn.json");
    fadeEasing_.SetAdaptValue(&alpha_);
}

void GameSceneFinish::Update([[maybe_unused]] float timeSpeed) {
    // HitStopタイマーをここでも更新することで、GameScenePlaying終了時に
    // HitStop中だった場合でも正しくタイムスケールが戻る
    pOwner_->GetGameObj().attackEffect_->Update();

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
            if (onFinished_) {
                onFinished_();
            }
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
