#include "TitleSceneController.h"
// Behavior

void TitleSceneController::Init(TitlePlayer* player) {

    // Playerセット
    player_ = player;

    // タイトルロゴ生成、初期化
    titleLogo_ = std::make_unique<TitleLogo>();
    titleLogo_->Init();

    // シーン遷移クラス生成、初期化
    sceneTransition_ = std::make_unique<TitleSceneTransition>();
    sceneTransition_->Init();

    isTitleLogoUpdate_ = false;
}

void TitleSceneController::Update() {
    // プレイヤー演出更新
    UpdatePlayerPerformance();

    // ロゴシーケンス更新
    UpdateLogoSequence();

    // シーン遷移管理更新
    sceneTransition_->Update();
}

void TitleSceneController::UpdatePlayerPerformance() {
    if (!player_) {
        return;
    }

    // タイトル用プレイヤー更新
    player_->TitleUpdate();

    // プレイヤーの特定行動をトリガーにロゴ表示開始
    if (player_->GetTitleBehavior() && player_->GetTitleBehavior()->IsTitlePlayerBehavior()) {
        if (!isTitleLogoUpdate_) {
            isTitleLogoUpdate_ = true;
        }
    }
}

void TitleSceneController::UpdateLogoSequence() {
    // タイトルロゴ更新
    if (isTitleLogoUpdate_) {
        titleLogo_->Update();
    }
}

bool TitleSceneController::IsTransitionComplete() const {
    return sceneTransition_->IsTransitionComplete();
}
