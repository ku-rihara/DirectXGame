/// behavior
#include "PlayerDeath.h"
#include "PlayerMove.h"

/// boss
#include "GameCamera/GameCamera.h"
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>

// 初期化
PlayerDeath::PlayerDeath(Player* player)
    : BasePlayerBehavior("PlayerDeath", player) {

    /// ===================================================
    /// 変数初期化
    /// ===================================================
    ChangeState([this](float timeSpeed) { StartDeathRendition(timeSpeed); });

    baseRotateYEase_.Init("PlayerDeathBaseRotateY.json");
    baseRotateYEase_.SetStartValue(pOwner_->GetBaseTransform().rotation_.y);
    baseRotateYEase_.SetAdaptValue(&tempBaseRotateY_);
   
    pOwner_->GetGameCamera()->Reset();
}

PlayerDeath ::~PlayerDeath() {
}

// 更新
void PlayerDeath::Update([[maybe_unused]] float timeSpeed) {
    if (currentState_) {
        currentState_(KetaEngine::Frame::DeltaTime());
    }
}

void PlayerDeath::StartDeathRendition(float timeSpeed) {
  
     timeSpeed;

    pOwner_->GetGameCamera()->PlayAnimation("GameOverCamera");
    ChangeState([this](float timeSpeed) { PlayerTurningAround(timeSpeed); });
}

void PlayerDeath::PlayerTurningAround(float timeSpeed) {
    baseRotateYEase_.Update(timeSpeed);
    pOwner_->SetRotationY(tempBaseRotateY_);
}

void PlayerDeath::ChangeState(std::function<void(float)> newState) {
    currentState_ = newState;
}

void PlayerDeath::Debug() {
}
