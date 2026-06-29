#include "PlayerBehaviorManager.h"
#include "Player/Behavior/ComboAttackBehavior/ComboAttackRoot.h"
#include "Player/Behavior/PlayerBehavior/PlayerDash.h"
#include "Player/Behavior/PlayerBehavior/PlayerDeath.h"
#include "Player/Behavior/PlayerBehavior/PlayerJump.h"
#include "Player/Behavior/PlayerBehavior/PlayerMove.h"
#include "Player/Behavior/PlayerBehavior/PlayerSpawn.h"
#include "Player/Player.h"

template <>
PlayerMove* PlayerBehaviorManager::GetAs<PlayerMove>() const {
    if (behavior_ && behavior_->GetBehaviorType() == BasePlayerBehavior::Type::Move) {
        return static_cast<PlayerMove*>(behavior_.get());
    }
    return nullptr;
}
template <>
PlayerDash* PlayerBehaviorManager::GetAs<PlayerDash>() const {
    if (behavior_ && behavior_->GetBehaviorType() == BasePlayerBehavior::Type::Dash) {
        return static_cast<PlayerDash*>(behavior_.get());
    }
    return nullptr;
}
template <>
PlayerJump* PlayerBehaviorManager::GetAs<PlayerJump>() const {
    if (behavior_ && behavior_->GetBehaviorType() == BasePlayerBehavior::Type::Jump) {
        return static_cast<PlayerJump*>(behavior_.get());
    }
    return nullptr;
}
template <>
PlayerDeath* PlayerBehaviorManager::GetAs<PlayerDeath>() const {
    if (behavior_ && behavior_->GetBehaviorType() == BasePlayerBehavior::Type::Death) {
        return static_cast<PlayerDeath*>(behavior_.get());
    }
    return nullptr;
}
template <>
PlayerSpawn* PlayerBehaviorManager::GetAs<PlayerSpawn>() const {
    if (behavior_ && behavior_->GetBehaviorType() == BasePlayerBehavior::Type::Spawn) {
        return static_cast<PlayerSpawn*>(behavior_.get());
    }
    return nullptr;
}
template <>
ComboAttackRoot* PlayerBehaviorManager::GetComboAs<ComboAttackRoot>() const {
    if (comboBehavior_ && comboBehavior_->GetComboType() == BaseComboAttackBehavior::Type::Root) {
        return static_cast<ComboAttackRoot*>(comboBehavior_.get());
    }
    return nullptr;
}

void PlayerBehaviorManager::Init(Player* player) {
    // 初期Behavior状態
    Change(std::make_unique<PlayerSpawn>(player));
    ChangeCombo(std::make_unique<ComboAttackRoot>(player));
}

/// ===============================================================================
/// Behaviorの切り替え
/// ===============================================================================
void PlayerBehaviorManager::Change(std::unique_ptr<BasePlayerBehavior> behavior) {
    behavior_ = std::move(behavior);
}

/// ===============================================================================
/// コンボBehaviorの切り替え
/// ===============================================================================
void PlayerBehaviorManager::ChangeCombo(std::unique_ptr<BaseComboAttackBehavior> behavior) {
    comboBehavior_ = std::move(behavior);
}

/// ===============================================================================
/// ComboBehaviorをRootに戻す
/// ===============================================================================
void PlayerBehaviorManager::ResetComboToRoot(Player* player) {
    ChangeCombo(std::make_unique<ComboAttackRoot>(player));
}

/// ===============================================================================
/// 死亡Behavior切り替え
/// ===============================================================================
void PlayerBehaviorManager::EnterDeath(Player* player) {
    ResetComboToRoot(player);
    Change(std::make_unique<PlayerDeath>(player));
}

void PlayerBehaviorManager::StartAutoDash(Player* player) {
    if (GetAs<PlayerMove>()) {
        Change(std::make_unique<PlayerDash>(player, true));
    }
}

void PlayerBehaviorManager::ClearForceDash() {
    if (auto* dash = GetAs<PlayerDash>()) {
        dash->SetForceDash(false);
    }
}

bool PlayerBehaviorManager::IsDashing() const {
    return GetAs<PlayerDash>() != nullptr;
}

bool PlayerBehaviorManager::IsStartDashing() const {
    auto* dash = GetAs<PlayerDash>();
    return dash && dash->IsStartDash();
}

bool PlayerBehaviorManager::IsAirborne() const {
    return GetAs<PlayerJump>() != nullptr;
}

bool PlayerBehaviorManager::IsComboRoot() const {
    return GetComboAs<ComboAttackRoot>() != nullptr;
}

bool PlayerBehaviorManager::IsDead() const {
    return GetAs<PlayerDeath>() != nullptr;
}

bool PlayerBehaviorManager::IsSpawning() const {
    return GetAs<PlayerSpawn>() != nullptr;
}
