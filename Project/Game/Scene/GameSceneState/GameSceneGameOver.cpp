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
    gameOverSprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));

    // フェードイン用イージング
    fadeEasing_.Init("GameSceneFadeIn.json");
    fadeEasing_.SetAdaptValue(&alpha_);
}

void GameSceneGameOver::Update([[maybe_unused]] float timeSpeed) {

    // HitStopタイマーを更新
    pOwner_->GetGameObj().attackEffect_->Update();

    // 死亡エフェクトを最後まで表示するためにプレイヤーを更新し続ける
    pOwner_->GetGameObj().player_->Update();

    auto* screen = pOwner_->GetGameObj().screenSprite_.get();

    switch (phase_) {
    case Phase::kSpriteAnim:
        // TimeUpスプライトのイージングアニメーション更新
        if (gameOverSprite_->GetSpriteEaseAnimationPlayer()->IsFinished()) {
            Vector2 finalPos = gameOverSprite_->GetSpriteEaseAnimationPlayer()->GetCurrentPosition();
            gameOverSprite_->transform_.pos = finalPos;
            gameOverSprite_->StopSpriteEaseAnimation();

            // 画面フェードイン開始
            screen->SetIsDraw(true);
            screen->SetAlpha(alpha_);
            phase_ = Phase::kFadeIn;
        }
        break;

    case Phase::kFadeIn:
        // 画面フェードイン更新
        fadeEasing_.Update(timeSpeed);
        screen->SetAlpha(alpha_);

        // フェードインが完了したらシーン遷移
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
