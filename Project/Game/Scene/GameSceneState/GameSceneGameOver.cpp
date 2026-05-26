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
    alpha_ = 0.7f;

    // TimeUpスプライト生成・アニメーション再生
    gameOverSprite_.reset(KetaEngine::Sprite::Create("TimeUp.dds"));
    gameOverSprite_->PlaySpriteEaseAnimation("finishSpritePos", "GameSceneFinish");
    gameOverSprite_->SetAnchorPoint(Vector2(0.5f, 0.5f));

    // フェードイン用イージング
    fadeEasing_.Init("GameSceneFadeIn.json");
    fadeEasing_.SetAdaptValue(&alpha_);

    // 最初の状態をスプライトアニメーション待ちに設定
    currentState_ = [this](float t) { StateSpriteAnim(t); };
}

void GameSceneGameOver::Update([[maybe_unused]] float timeSpeed) {
    // HitStopタイマーを更新
    pOwner_->GetGameObj().attackEffect_->Update();

    // 死亡エフェクトを最後まで表示するためにプレイヤーを更新し続ける
    pOwner_->GetGameObj().player_->Update();

    currentState_(timeSpeed);

    ViewUpDate();
}

void GameSceneGameOver::StateSpriteAnim([[maybe_unused]] float timeSpeed) {
    if (!gameOverSprite_->GetSpriteEaseAnimationPlayer()->IsFinished()) { 
        return; 
    }

    // スプライトの最終位置を取得してからアニメーションを停止
    Vector2 finalPos = gameOverSprite_->GetSpriteEaseAnimationPlayer()->GetCurrentPosition();
    gameOverSprite_->transform_.pos = finalPos;
    gameOverSprite_->StopSpriteEaseAnimation();

    // 画面フェードイン開始
    auto* screen = pOwner_->GetGameObj().screenSprite_.get();
    screen->SetIsDraw(true);
    screen->SetAlpha(alpha_);

    // フェードイン状態に遷移
    currentState_ = [this](float t) { StateFadeIn(t); };
}

void GameSceneGameOver::StateFadeIn(float timeSpeed) {
    // 画面フェードイン更新
    fadeEasing_.Update(timeSpeed);
    pOwner_->GetGameObj().screenSprite_->SetAlpha(alpha_);

    // フェードインが完了したらシーン遷移
    if (!fadeEasing_.IsFinished()) {
        return; 
    }

    // シーン遷移
    currentState_ = [](float) {};
    KetaEngine::SceneManager::GetInstance()->ChangeScene("RESULT");
}

void GameSceneGameOver::Debug() {
}
