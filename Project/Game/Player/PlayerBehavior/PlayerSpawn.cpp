/// behavior
#include "PlayerSpawn.h"
#include "PlayerMove.h"

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

    spawnEase_.Init("Spawn");
    spawnEase_.ApplyFromJson("SpawnDissolve.json");
    spawnEase_.SaveAppliedJsonFileName();
    spawnEase_.SetAdaptValue(&tempDessolve_);
    spawnEase_.Reset();

    spawnEase_.SetOnWaitEndCallback([this]() {
        step_ = Step::END;
    });

    speed_ = pPlayerParameter_->GetParamaters().moveSpeed;

    step_ = Step::WAIT;
}

PlayerSpawn ::~PlayerSpawn() {
}

// 更新
void PlayerSpawn::Update() {

    switch (step_) {

        ///================================================================
        ///
        ///================================================================
    case PlayerSpawn::Step::WAIT:

        pPlayer_->DissolveUpdate(1.0f);
        pPlayer_->GetLeftHand()->DissolveAdapt(1.0f);
        pPlayer_->GetRightHand()->DissolveAdapt(1.0f);
        pPlayer_->GetLeftHand()->SetIsEmit(false);
        pPlayer_->GetRightHand()->SetIsEmit(false);
        pPlayer_->SetShadowFrag(false);
        startWaitTime_ += Frame::DeltaTime();
        if (startWaitTime_ < pPlayerParameter_->GetParamaters().spawnParam.waitTime_) {
            break;
        }
        pPlayer_->SetShadowFrag(true);
        step_ = Step::SPAWN;
        break;

        ///================================================================
        ///
        ///================================================================
    case PlayerSpawn::Step::SPAWN:
        spawnEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->DissolveUpdate(tempDessolve_);
        pPlayer_->GetLeftHand()->DissolveAdapt(tempDessolve_);
        pPlayer_->GetRightHand()->DissolveAdapt(tempDessolve_);
        break;

        ///================================================================
        ///
        ///================================================================
    case PlayerSpawn::Step::END:
        pPlayer_->GetLeftHand()->SetIsEmit(true);
        pPlayer_->GetRightHand()->SetIsEmit(true);
        pPlayer_->ChangeBehavior(std::make_unique<PlayerMove>(pPlayer_));
        break;
    default:
        break;
    }
}

void PlayerSpawn::Debug() {
    ImGui::Text("spawn");
}
