/// behavior
#include "PlayerMove.h"
#include "PlayerJump.h"

/// boss
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
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

    // MoveAnimation
    easeAnimationPlayer_.Init();

    // 移動スピード、アニメーションステップセット
    speed_ = pPlayerParameter_->GetParameters().moveSpeed;
    animationStep_ = AnimationStep::INIT;
}

PlayerMove ::~PlayerMove() {
}

// 更新
void PlayerMove::Update([[maybe_unused]] float timeSpeed) {

    // イージングアニメーション
    MoveAnimation();
    WaitAnimation();

    // ダッシュ
    if (pOwner_->GetInput().IsDashInput() || forceDash_) {
        isDashing_ = true;
        pOwner_->Move(pPlayerParameter_->GetParameters().moveSpeed *
                      pPlayerParameter_->GetParameters().dashSpeedMultiplier);
    } else {
        isDashing_ = false;
        pOwner_->Move(pPlayerParameter_->GetParameters().moveSpeed);
    }

    // 　ジャンプに切り替え
    if (pOwner_->GetInput().IsJumpKeyPressed()) {
        pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_, pPlayerParameter_->GetParameters().normalJump.jumpSpeed));
    } else {
        JumpForJoyState(); // コントローラジャンプ
    }
}

void PlayerMove::JumpForJoyState() {

    if (!pOwner_->GetInput().IsJumpPadTriggered()) {
        return;
    }

    pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_, pPlayerParameter_->GetParameters().normalJump.jumpSpeed));
}

void PlayerMove::MoveAnimation() {
    if (!pOwner_->CheckIsMoving()) {
        return;
    }

  
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
