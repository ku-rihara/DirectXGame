/// behavior
#include "PlayerJump.h"
#include "PlayerMove.h"

/// boss
#include "GameCamera/GameCamera.h"
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
#include <imgui.h>

// 初期化
PlayerJump::PlayerJump(Player* player, const bool& skipJump)
    : BasePlayerBehavior("PlayerJump", player), skipJump_(skipJump) {

    /// ===================================================
    /// 変数初期化
    /// ===================================================

    // ジャンプをスキップする場合は速度を0にして落下のみ
    if (skipJump_) {
        speed_ = 0.0f;
        step_  = Step::FALL_ONLY;
    } else {
        speed_ = pPlayerParameter_->GetParamaters().normalJump.jumpSpeed;
        step_  = Step::START;
    }
}

PlayerJump ::~PlayerJump() {
}

// 更新
void PlayerJump::Update() {

    switch (step_) {
    case PlayerJump::Step::START:
        if (skipJump_) {
            pPlayer_->GetGameCamera()->PlayAnimation("PlayerJunmp");
            pPlayer_->GetJumpAttackUI()->StartOpen();
        }
        step_ = Step::JUMP;
        break;

    case PlayerJump::Step::JUMP:
        // ジャンプ中の移動
        pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed);

        // ジャンプ処理
        pPlayer_->Jump(speed_, pPlayerParameter_->GetParamaters().normalJump.fallSpeedLimit, pPlayerParameter_->GetParamaters().normalJump.gravity);

        // 着地、通常移動に戻る
        if (pPlayer_->GetTransform().translation_.y > pPlayerParameter_->GetParamaters().startPos_.y) {
            return;
        }
        pPlayer_->GetJumpAttackUI()->StartClose();
        // behavior切り替え
        pPlayer_->ChangeBehavior(std::make_unique<PlayerMove>(pPlayer_));
        break;

    case PlayerJump::Step::FALL_ONLY:
        // 横方向の移動は可能
        pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed);

        // 落下処理のみ
        pPlayer_->Fall(
            speed_,
            pPlayerParameter_->GetParamaters().normalJump.fallSpeedLimit,
            pPlayerParameter_->GetParamaters().normalJump.gravity,
            false);

        // 着地、通常移動に戻る
        if (pPlayer_->GetTransform().translation_.y <= pPlayerParameter_->GetParamaters().startPos_.y) {
            pPlayer_->ChangeBehavior(std::make_unique<PlayerMove>(pPlayer_));
        }
        break;

    default:
        break;
    }
}

void PlayerJump::Debug() {
   
}