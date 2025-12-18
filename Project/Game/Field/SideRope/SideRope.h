#pragma once

#include "3d/WorldTransform.h"
#include "Collider/AABBCollider.h"
#include <variant>

class BaseEnemy;
/// <summary>
/// ゲームフィールドクラス
/// </summary>
class SideRope : public KetaEngine::AABBCollider {
public:
    using ReboundTarget = std::variant<BaseEnemy*>;

public:
    SideRope()  = default;
    ~SideRope() = default;

    // 初期化、更新
    void Init();
    void Update();

    // 反発処理
    void Rebound(const ReboundTarget& target);

    /// collision
    Vector3 GetCollisionPos() const override;

private:
    KetaEngine::WorldTransform transform_;
};