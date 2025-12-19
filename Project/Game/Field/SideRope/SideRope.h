#pragma once

#include "3d/WorldTransform.h"
#include "Collider/AABBCollider.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <string>
#include <variant>

enum class SideRopeType : int32_t {
    FRONT,
    LEFT,
    RIGHT,
    BACK,
    COUNT,
};

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
    void Init(int32_t type);
    void Update();

    // 反発処理
    void Rebound(const ReboundTarget& target);

    /// collision
    Vector3 GetCollisionPos() const override;

    // parameterEdit
    void AdjustParam();

private:
    std::string SetGroupNameByType();

    // Parameter
    void RegisterParams();

private:
    // global parameter
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;

    KetaEngine::WorldTransform transform_;
    SideRopeType type_;

    // parameter
    Vector3 collisionSize_;
    Vector3 collisionPos_;
};