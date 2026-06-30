/// behavior
#include "PlayerMove.h"
#include "PlayerDash.h"
#include "PlayerJump.h"

/// player
#include "Player/Player.h"
#include "Player/Components/Animation/PlayerAnimator.h"
/// frame
/// imgui
#include <imgui.h>

PlayerMove::PlayerMove(Player* player)
    : BasePlayerBehavior("PlayerMove", player) {

    wasMoving_ = !pOwner_->GetInput().IsMoving();
}

PlayerMove::~PlayerMove() {
}

void PlayerMove::Update([[maybe_unused]] float timeSpeed) {

    // 押し出し等でY座標が浮いたままになるのを防ぐため、地上状態ではYをリセットする
    pOwner_->GetPlayerAnimator().ResetPositionY();

    CheckAndSwitchAnimation();

    // ダッシュ入力 → PlayerDash へ
    if (pOwner_->GetInput().IsDashInput()) {
        pOwner_->ChangeBehavior(std::make_unique<PlayerDash>(pOwner_));
        return;
    }

    pOwner_->Move(pPlayerParameter_->GetParameters().moveSpeed);

    // ジャンプに切り替え
    if (pOwner_->GetInput().IsJumpKeyTriggered()) {
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
    bool isMoving = pOwner_->GetInput().IsMoving();

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
