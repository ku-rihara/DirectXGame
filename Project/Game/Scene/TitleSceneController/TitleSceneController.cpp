#include "TitleSceneController.h"
#include "Frame/Frame.h"
#include "Player/TitleBehavior/TitleRightPunch.h"

void TitleSceneController::Init(Player* player) {
    player_ = player;

    titleLogo_ = std::make_unique<TitleLogo>();
    titleLogo_->Init();

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
    if (!player_)
        return;

    player_->TitleUpdate();

    // プレイヤーの特定行動をトリガーにロゴ表示開始
    if (dynamic_cast<TitleRightPunch*>(player_->GetTitleBehavior())) {
        if (!isTitleLogoUpdate_) {
            isTitleLogoUpdate_ = true;
        }
    }
}

void TitleSceneController::UpdateLogoSequence() {
    if (isTitleLogoUpdate_) {
        titleLogo_->Update();
    }
}

bool TitleSceneController::IsTransitionComplete() const {
    return sceneTransition_->IsTransitionComplete();
}

const std::unique_ptr<KetaEngine::Sprite>& TitleSceneController::GetScreenSprite() const {
    return sceneTransition_->GetScreenSprite();
}