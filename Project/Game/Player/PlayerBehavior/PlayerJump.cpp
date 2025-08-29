/// behavior
#include "PlayerJump.h"
#include "PlayerMove.h"

/// boss
#include "Player/Player.h"
#include"GameCamera/GameCamera.h"
/// frame
#include "Frame/Frame.h"
#include <imgui.h>

// 初期化
PlayerJump::PlayerJump(Player* player)
    : BasePlayerBehavior("PlayerJump", player) {

    /// ===================================================
    /// 変数初期化
    /// ===================================================

    speed_ = pPlayerParameter_->GetParamaters().normalJump.jumpSpeed;
    step_  = Step::START;
}

PlayerJump ::~PlayerJump() {
}

// 更新
void PlayerJump::Update() {

    switch (step_) {
    case PlayerJump::Step::START:
        pPlayer_->GetGameCamera()->PlayAnimation("PlayerJunmp");
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

        // behavior切り替え
        pPlayer_->ChangeBehavior(std::make_unique<PlayerMove>(pPlayer_));
        break;
    default:
        break;
    }

   
}

void PlayerJump::Debug() {
    ImGui::Text("Jump");
}