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
    moveEase_ = std::make_unique<KetaEngine::EasingSequence>();
    moveEase_->AddStep("PlayerMovePosY.json", &tempPosY_);
    moveEase_->AddStep("PlayerLandingScale.json", &tempScale_);
    moveEase_->SetLoop(true);

    speed_ = pPlayerParameter_->GetParamaters().moveSpeed;

    animationStep_ = AnimationStep::INIT;
}

PlayerMove ::~PlayerMove() {
}

// 更新
void PlayerMove::Update([[maybe_unused]] float timeSpeed) {

    // アニメーション
    MoveAnimation();
    WaitAnimation();

    if ((KetaEngine::Input::IsPressPad(0, GamepadButton::B))) {
        pOwner_->Move(pPlayerParameter_->GetParamaters().moveSpeed * 2.4f);
    } else {
        pOwner_->Move(pPlayerParameter_->GetParamaters().moveSpeed);
    }

    // 　ジャンプに切り替え
    if (KetaEngine::Input::GetInstance()->PushKey(KeyboardKey::J)) {
        pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_));
    } else {
        JumpForJoyState(); // コントローラジャンプ
    }
}

void PlayerMove::JumpForJoyState() {

    if (!(KetaEngine::Input::IsTriggerPad(0, GamepadButton::A))) {
        return;
    }

    pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_));
}

void PlayerMove::MoveAnimation() {
    if (!pOwner_->CheckIsMoving()) {
        return;
    }

    ///============================================================================
    /// 移動アニメーション
    ///============================================================================
    moveEase_->Update(KetaEngine::Frame::DeltaTimeRate());
    pOwner_->SetHeadPosY(tempPosY_);
    pOwner_->SetHeadScale(tempScale_);
}

void PlayerMove::WaitAnimation() {

    if (pOwner_->CheckIsMoving()) {
        return;
    }

    ///============================================================================
    /// 待機アニメーション
    ///============================================================================
    waitEase_.Update(KetaEngine::Frame::DeltaTimeRate());
    pOwner_->SetScaleY(tempWaitScaleY_);
}

void PlayerMove::Debug() {
    ImGui::Text("Root");
}
