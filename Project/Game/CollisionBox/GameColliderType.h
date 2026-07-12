#pragma once

#include "Collider/BaseCollider.h"
#include <cstdint>

/// <summary>
/// ゲーム固有のコライダー種別
/// </summary>
enum class GameColliderType : uint32_t {
    None           = 0,
    Player         = 1u << 0,
    PlayerAttack   = 1u << 1,
    Enemy          = 1u << 2,
    LeaderEnemy    = 1u << 3,
    EntourageEnemy = 1u << 4,
};

inline uint32_t operator|(GameColliderType lhs, GameColliderType rhs) {
    return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}

inline uint32_t operator|(uint32_t lhs, GameColliderType rhs) {
    return lhs | static_cast<uint32_t>(rhs);
}

/// <summary>
/// コライダーが指定した種別のフラグを持っているか判定する
/// </summary>
inline bool HasColliderType(const KetaEngine::BaseCollider* collider, GameColliderType type) {
    if (!collider) {
        return false;
    }
    return (collider->GetTypeID() & static_cast<uint32_t>(type)) != 0;
}
