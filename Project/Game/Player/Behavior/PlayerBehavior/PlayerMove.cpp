/// behavior
#include "PlayerMove.h"
#include "PlayerDash.h"
#include "PlayerJump.h"

/// player
#include "Player/Player.h"
#include "Player/Components/Animation/PlayerAnimator.h"
/// frame
#include "Frame/Frame.h"
/// imgui
#include <imgui.h>

PlayerMove::PlayerMove(Player* player)
    : BasePlayerBehavior("PlayerMove", player) {
    // 攻撃等から復帰した時に確実にアニメーションを再開するため
    // 現在の移動状態と逆の値を入れて次のUpdateで必ず差分が生じるようにする
    wasMoving_ = !pOwner_->CheckIsMoving();
}

PlayerMove::~PlayerMove() {
}

void PlayerMove::Update([[maybe_unused]] float timeSpeed) {

    CheckAndSwitchAnimation();

    // ダッシュ入力 → PlayerDash へ
    if (pOwner_->GetInput().IsDashInput()) {
        pOwner_->ChangeBehavior(std::make_unique<PlayerDash>(pOwner_));
        return;
    }

    pOwner_->Move(pPlayerParameter_->GetParameters().moveSpeed);

    // ジャンプに切り替え
    if (pOwner_->GetInput().IsJumpKeyPressed()) {
        pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_, pPlayerParameter_->GetParameters().normalJump.jumpSpeed));
    } else {
        JumpForJoyState();
    }
}

void PlayerMove::JumpForJoyState() {
    if (!pOwner_->GetInput().IsJumpPadTriggered()) {
        return;
    }
    pOwner_->ChangeBehavior(std::make_unique<PlayerJump>(pOwner_, pPlayerParameter_->GetParameters().normalJump.jumpSpeed));
}

void PlayerMove::CheckAndSwitchAnimation() {
    // 移動状態を取得
    bool isMoving = pOwner_->CheckIsMoving();

    // 移動状態が変わってるかチェック
    if (isMoving == wasMoving_) {
        return;
    }
    wasMoving_ = isMoving;

    if (isMoving) {
        pOwner_->GetPlayerAnimator().PlayMoveAnimation();
    } else {
        pOwner_->GetPlayerAnimator().PlayWaitAnimation();
    }
}

void PlayerMove::Debug() {
    ImGui::Text("Root");
}
