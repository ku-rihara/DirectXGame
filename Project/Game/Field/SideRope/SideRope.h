#pragma once

#include "3d/WorldTransform.h"
#include "Collider/AABBCollider.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "RopeBoundParam.h"
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
class SideRopeReboundSystem;
    /// <summary>
/// ゲームフィールドクラス
/// </summary>
class SideRope : public KetaEngine::AABBCollider {
public:
    SideRope()  = default;
    ~SideRope() = default;

    // 初期化、更新
    void Init(int32_t type);
    void Update();

    /// collision
    Vector3 GetCollisionPos() const override;

    // parameterEdit
    void AdjustParam();
    Vector3 CalculateRopeNormal() const;

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

    RopeBoundParam ropeParam_;

public:
    const RopeBoundParam GetRopeParam() const { return ropeParam_; } 
    void SetRopeParam(const RopeBoundParam& param) { ropeParam_ = param; }
};