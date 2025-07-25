/// behavior
#include "PlayerSpawn.h"
#include "PlayerJump.h"

/// boss
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>

// 初期化
PlayerSpawn::PlayerSpawn(Player* player)
    : BasePlayerBehavior("PlayerSpawn", player) {

    /// ===================================================
    /// 変数初期化
    /// ===================================================

    waitEase_.Init("PlayerWait");
    waitEase_.ApplyFromJson("PlayerWait.json");
    waitEase_.SaveAppliedJsonFileName();
    waitEase_.SetAdaptValue(&tempWaitScaleY_);
    waitEase_.Reset();

    waitEase_.SetOnWaitEndCallback([this]() {
        waitEase_.Reset();
    });

    speed_ = pPlayerParameter_->GetParamaters().moveSpeed;

    animationStep_ = AnimationStep::INIT;
}

PlayerSpawn ::~PlayerSpawn() {
}

// 更新
void PlayerSpawn::Update() {

    waitEase_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetScaleY(tempWaitScaleY_);
   
}


void PlayerSpawn::Debug() {
    ImGui::Text("spawn");
}
