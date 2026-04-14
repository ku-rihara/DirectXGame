/// behavior
#include "PlayerDash.h"
#include "PlayerJump.h"
#include "PlayerMove.h"

/// boss
#include "Player/Player.h"
/// imgui
#include <imgui.h>

PlayerDash::PlayerDash(Player* player, bool forceDash)
    : BasePlayerBehavior("PlayerDash", player), forceDash_(forceDash) {

    // 最初のダッシュ状態からスタート
    currentState_ = [this]() {
        StartDash();
    };
}

void PlayerDash::Update([[maybe_unused]] float timeSpeed) {
    currentState_();
}

void PlayerDash::StartDash() {
    // DashStart イージングアニメーション再生
    pOwner_->GetPlayerAnimator().PlayDashStartAnimation();
    // ダッシュエフェクト開始
    pOwner_->GetEffects()->DashEffectStart();

    currentState_ = [this]() {
        UpdateNormalDash();
    };
}

void PlayerDash::UpdateStartDash() {
    if (pOwner_->GetPlayerAnimator().IsDashStartAnimationFinished()) {
        currentState_ = [this]() {
            UpdateNormalDash();
        };
    }
}

void PlayerDash::UpdateNormalDash() {
    // 強制ダッシュでなく、入力も離れていたら通常移動へ
    if (!forceDash_ && !pOwner_->GetInput().IsDashInput()) {
        pOwner_->ChangeBehavior(std::make_unique<PlayerMove>(pOwner_));
        return;
    }

    // ダッシュ移動
    pOwner_->Move(pPlayerParameter_->GetParameters().moveSpeed * pPlayerParameter_->GetParameters().dashSpeedMultiplier);

    // ジャンプ
    if (pOwner_->GetInput().IsJumpKeyPressed()) {
        pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_, pPlayerParameter_->GetParameters().normalJump.jumpSpeed));
        return;
    }
    JumpForJoyState();
}

void PlayerDash::JumpForJoyState() {
    if (!pOwner_->GetInput().IsJumpPadTriggered()) {
        return;
    }
    pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_, pPlayerParameter_->GetParameters().normalJump.jumpSpeed));
}

void PlayerDash::Debug() {
    ImGui::Text("Dash");
}
