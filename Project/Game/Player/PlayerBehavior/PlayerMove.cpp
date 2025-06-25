/// behavior
#include "PlayerMove.h"
#include "PlayerJump.h"

/// boss
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
/// inupt
#include "JoyState/JoyState.h"
/// imgui
#include <imgui.h>

// 初期化
PlayerMove::PlayerMove(Player* player)
    : BasePlayerBehavior("PlayerMove", player) {

    /// ===================================================
    /// 変数初期化
    /// ===================================================
    /*waitEase_.maxTime = 0.4f;
   easeDirection_ = 1.0f;*/

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

PlayerMove ::~PlayerMove() {
}

// 更新
void PlayerMove::Update() {

    // アニメーション
    MoveAnimation();
    WaitAnimation();

    if ((Input::IsPressPad(0, XINPUT_GAMEPAD_Y))) {
        pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed * 2.4f);
    } else {
        pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed);
    }

    // 　ジャンプに切り替え
    if (Input::GetInstance()->PushKey(DIK_J)) {
        pPlayer_->ChangeBehavior(std::make_unique<PlayerJump>(pPlayer_));
    } else {
        JumpForJoyState(); // コントローラジャンプ
    }
}

void PlayerMove::JumpForJoyState() {

    if (!(Input::IsTriggerPad(0, XINPUT_GAMEPAD_A)))
        return;

    pPlayer_->ChangeBehavior(std::make_unique<PlayerJump>(pPlayer_));
}

void PlayerMove::MoveAnimation() {
    if (!pPlayer_->GetIsMoving())
        return;

    // switch (animationStep_) {
    // case AnimationStep::INIT:
    //     ///============================================================================
    //     /// 初期化
    //     ///============================================================================
    //     animationCollTime_ = 0.5f;
    //    /* moveEase_.time   = 0.0f;
    //     waitEase_.time   = 0.0f;
    //     moveEase_.maxTime  = 0.4f;*/
    //
    //     animationStep_   = AnimationStep::UPDATE;
    //     break;
    // case AnimationStep::UPDATE:

    //    ///============================================================================
    //    /// アニメーション
    //    ///============================================================================

    //   /* moveEase_.time += Frame::DeltaTime();
    //    if (moveEase_.time >= moveEase_.maxTime) {
    //        moveEase_.time = 0.0f;
    //        animationStep_ = AnimationStep::COOLING;
    //    }*/

    //     pPlayer_->SetScaleY(
    //        EaseInCubic<float>(Vector3::UnitVector().y,Vector3::UnitVector().y - 0.1f, moveEase_.time, moveEase_.maxTime));
    //    break;
    // case AnimationStep::COOLING:
    //    ///============================================================================
    //    /// クールタイム
    //    ///============================================================================

    //    animationCollTime_ -= Frame::DeltaTime();
    //    if (animationCollTime_ <= 0.0f) {
    //        animationStep_ = AnimationStep::INIT;
    //    }

    //    break;
    //}
}

void PlayerMove::WaitAnimation() {

    if (pPlayer_->GetIsMoving())
        return;

    ///============================================================================
    /// 待機アニメーション
    ///============================================================================
    waitEase_.Update(Frame::DeltaTimeRate());
    waitEase_.UpdateWait(Frame::DeltaTimeRate());
    pPlayer_->SetScaleY(tempWaitScaleY_);
    
}

void PlayerMove::Debug() {
    ImGui::Text("Root");
}
