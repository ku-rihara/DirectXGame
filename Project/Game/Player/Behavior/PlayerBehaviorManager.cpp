#include "PlayerBehaviorManager.h"
#include "Player/Player.h"
#include "Player/Behavior/ComboAttackBehavior/ComboAttackRoot.h"
#include "Player/Behavior/PlayerBehavior/PlayerDash.h"
#include "Player/Behavior/PlayerBehavior/PlayerDeath.h"
#include "Player/Behavior/PlayerBehavior/PlayerJump.h"
#include "Player/Behavior/PlayerBehavior/PlayerMove.h"
#include "Player/Behavior/PlayerBehavior/PlayerSpawn.h"

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
