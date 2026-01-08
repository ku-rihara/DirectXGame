/// behavior
#include "PlayerDeath.h"
#include "PlayerMove.h"

/// boss
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>

// 初期化
PlayerDeath::PlayerDeath(Player* player)
    : BasePlayerBehavior("PlayerDeath", player) {

    /// ===================================================
    /// 変数初期化
    /// ===================================================
   /* ChangeState([this]() { FallOnlyState(); });*/
}

PlayerDeath ::~PlayerDeath() {
}

// 更新
void PlayerDeath::Update([[maybe_unused]] float timeSpeed) {
    if (currentState_) {
        currentState_();
    }
  
}

void PlayerDeath::ChangeState(std::function<void()> newState) {
    currentState_ = newState;
}

void PlayerDeath::Debug() {
   
}
