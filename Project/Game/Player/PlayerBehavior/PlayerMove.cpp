/// behavior
#include "PlayerMove.h"
#include "PlayerJump.h"

/// boss
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
/// inupt
#include "input/Input.h"
/// imgui
#include <imgui.h>

// 初期化
PlayerMove::PlayerMove(Player* player)
    : BasePlayerBehavior("PlayerMove", player) {

    /// ===================================================
    /// 変数初期化
    /// ===================================================

    // waitEase
    waitEase_.Init("PlayerWait.json");
    waitEase_.SetAdaptValue(&tempWaitScaleY_);

    waitEase_.SetOnWaitEndCallback([this]() {
        waitEase_.Reset();
    });
    
    // moveEase
    moveEase_ = std::make_unique<EasingSequence>();
    moveEase_->AddStep("PlayerMovePosY.json", &tempPosY_);
    moveEase_->AddStep("PlayerLandingScale.json", &tempScale_);
    moveEase_->SetLoop(true);

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

    if (!(Input::IsTriggerPad(0, XINPUT_GAMEPAD_A))) {
        return;
    }

    pPlayer_->ChangeBehavior(std::make_unique<PlayerJump>(pPlayer_));
}

void PlayerMove::MoveAnimation() {
    if (!pPlayer_->CheckIsMoving()) {
        return;
    }

    ///============================================================================
    /// 移動アニメーション
    ///============================================================================
    moveEase_->Update(Frame::DeltaTimeRate());
    pPlayer_->SetHeadPosY(tempPosY_);
    pPlayer_->SetHeadScale(tempScale_);
}

void PlayerMove::WaitAnimation() {

    if (pPlayer_->CheckIsMoving()) {
        return;
    }

    ///============================================================================
    /// 待機アニメーション
    ///============================================================================
    waitEase_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetScaleY(tempWaitScaleY_);
}

void PlayerMove::Debug() {
    ImGui::Text("Root");
}
