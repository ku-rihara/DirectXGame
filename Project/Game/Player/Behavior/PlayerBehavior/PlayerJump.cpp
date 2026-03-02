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
PlayerJump::PlayerJump(Player* player, float initSpeed)
    : BasePlayerBehavior("PlayerJump", player) {

    /// ===================================================
    /// 変数初期化
    /// ===================================================
    speed_ = initSpeed;

    if (speed_==0.0f) {
        ChangeState([this]() { FallOnlyState(); });
    } else {
        pOwner_->GetGameCamera()->PlayAnimation("PlayerJunmp", false);
        pOwner_->GetObject3D()->transform_.PlayObjEaseAnimation("JumpRotation", "Player");

        ChangeState([this]() { StartState(); });
    }
}

PlayerJump::~PlayerJump() {
}

// 更新
void PlayerJump::Update([[maybe_unused]] float timeSpeed) {
    if (currentState_) {
        currentState_();
    }
}

void PlayerJump::Debug() {
}

void PlayerJump::ChangeState(std::function<void()> newState) {
    currentState_ = newState;
}

void PlayerJump::StartState() {
   
    ChangeState([this]() { JumpState(); });
}

void PlayerJump::JumpState() {
    // ジャンプ中の移動
    pOwner_->Move(pPlayerParameter_->GetParameters().moveSpeed);

    // ジャンプ処理
    pOwner_->Jump(
        speed_,
        pPlayerParameter_->GetParameters().normalJump.fallSpeedLimit,
        pPlayerParameter_->GetParameters().normalJump.gravity);

    // 着地、通常移動に戻る
    if (pOwner_->GetBaseTransform().translation_.y <= pPlayerParameter_->GetParameters().startPos_.y) {
        pOwner_->ChangeBehavior(std::make_unique<PlayerMove>(pOwner_));
    }
}

void PlayerJump::FallOnlyState() {
    // 横方向の移動は可能
    pOwner_->Move(pPlayerParameter_->GetParameters().moveSpeed);

    // 落下処理のみ
    pOwner_->Fall(
        speed_,
        pPlayerParameter_->GetParameters().normalJump.fallSpeedLimit,
        pPlayerParameter_->GetParameters().normalJump.gravity,
        false);

    // 着地、通常移動に戻る
    if (pOwner_->GetBaseTransform().translation_.y <= pPlayerParameter_->GetParameters().startPos_.y) {
        pOwner_->ChangeBehavior(std::make_unique<PlayerMove>(pOwner_));
    }
}