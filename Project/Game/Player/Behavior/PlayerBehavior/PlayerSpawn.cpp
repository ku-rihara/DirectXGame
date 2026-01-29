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

    waitEase_.Init("PlayerWait.json");
    waitEase_.SetAdaptValue(&tempWaitScaleY_);
    waitEase_.Reset();

    waitEase_.SetOnWaitEndCallback([this]() {
        waitEase_.Reset();
    });

    spawnEase_.Init("SpawnDissolve.json");
    spawnEase_.SetAdaptValue(&tempDessolve_);
    spawnEase_.Reset();

    spawnEase_.SetOnWaitEndCallback([this]() {
        step_ = Step::END;
    });

    speed_ = pPlayerParameter_->GetParameters().moveSpeed;

    step_ = Step::WAIT;
}

PlayerSpawn ::~PlayerSpawn() {
}

// 更新
void PlayerSpawn::Update([[maybe_unused]] float timeSpeed) {

    switch (step_) {

        ///================================================================
        ///
        ///================================================================
    case PlayerSpawn::Step::WAIT:

       
        startWaitTime_ += KetaEngine::Frame::DeltaTime();
        if (startWaitTime_ < pPlayerParameter_->GetParameters().spawnParam.waitTime_) {
            break;
        }
        pOwner_->SetShadowFrag(true);
        step_ = Step::SPAWN;
        break;

        ///================================================================
        ///
        ///================================================================
    case PlayerSpawn::Step::SPAWN:
        spawnEase_.Update(KetaEngine::Frame::DeltaTimeRate());
        pOwner_->DissolveUpdate(tempDessolve_);
        pOwner_->GetLeftHand()->DissolveAdapt(tempDessolve_);
        pOwner_->GetRightHand()->DissolveAdapt(tempDessolve_);
        break;

        ///================================================================
        ///
        ///================================================================
    case PlayerSpawn::Step::END:
        pOwner_->GetLeftHand()->SetIsEmit(true);
        pOwner_->GetRightHand()->SetIsEmit(true);
        pOwner_->ChangeBehavior(std::make_unique<PlayerMove>(pOwner_));
        break;
    default:
        break;
    }
}

void PlayerSpawn::Debug() {
    ImGui::Text("spawn");
}
