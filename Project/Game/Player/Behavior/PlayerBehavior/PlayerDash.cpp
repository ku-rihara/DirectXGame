/// behavior
#include "PlayerDash.h"
#include "PlayerJump.h"
#include "PlayerMove.h"

/// boss
#include "Player/Player.h"
/// engine
#include "Frame/Frame.h"
/// imgui
#include <imgui.h>

PlayerDash::PlayerDash(Player* player, bool forceDash)
    : BasePlayerBehavior("PlayerDash", player), forceDash_(forceDash) {

    dashEasing_.Init("PlayerDashStart.json");
    currentState_ = [this]() { StartDash(); };
}

void PlayerDash::Update([[maybe_unused]] float timeSpeed) {
    currentState_();
}

void PlayerDash::StartDash() {
    const auto& dashParam = pPlayerParameter_->GetParameters().dashParam;

    // ダッシュ開始アニメーションとエフェクト
    pOwner_->GetPlayerAnimator().PlayDashStartAnimation();
    pOwner_->GetEffects()->DashEffectStart();

    // ダッシュ開始位置と終了位置を計算
    Vector3 start = pOwner_->GetWorldPosition();
    Vector3 end   = start + pOwner_->GetBaseTransform().CalcForwardOffset({0.0f, 0.0f, dashParam.distance});

    // ダッシュイージングのセット
    dashCurrentPos_ = start;
    dashEasing_.SetStartValue(start);
    dashEasing_.SetEndValue(end);
    dashEasing_.SetAdaptValue(&dashCurrentPos_);
    dashEasing_.Reset();

    currentState_ = [this]() { UpdateStartDash(); };
}

void PlayerDash::UpdateStartDash() {
    dashEasing_.Update(KetaEngine::Frame::DeltaTime());
    pOwner_->SetWorldPosition(dashCurrentPos_);
    if (dashEasing_.IsFinished()) {
        currentState_ = [this]() { UpdateNormalDash(); };
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
