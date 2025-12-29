#pragma once

#include "Vector3.h"
#include "RopeBoundParam.h"
#include <functional>

/// <summary>
/// ロープ反発処理のシステム
/// </summary>
class SideRopeReboundSystem {
public:
    // フェーズ
    enum class Phase {
        STRETCH,
        REBOUND,
        END,
    };

    SideRopeReboundSystem()  = default;
    ~SideRopeReboundSystem() = default;

    // 反発初期化
    void Init(
        const Vector3& initialVelocity,
        const Vector3& ropeNormal,
       const RopeBoundParam&ropeParam);

    // 更新
    Vector3 Update(float deltaTime);

    Vector3 UpdateStretch(float deltaTime);
    Vector3 UpdateRebound(float deltaTime);
    Vector3 UpdateEnd(float deltaTime);

private:
    Vector3 currentVelocity_;
    Vector3 ropeNormal_;

    float stretchDistance_;
    float currentDelay_;
    float reboundDistance_; 

    RopeBoundParam ropeParam_;

    Phase phase_;
    bool isActive_ = false;

    std::function<Vector3(float)> updateFunc_;

public:
    bool IsActive() const;
    Phase GetPhase() const;
    float GetStretchRate() const;
};