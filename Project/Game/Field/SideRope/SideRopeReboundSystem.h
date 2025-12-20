#pragma once

#include "Vector3.h"
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
        float maxStretch,
        float stretchResistance,
        float reboundRate,
        float delayTime);

    // 更新
    Vector3 Update(float deltaTime);

    Vector3 UpdateStretch(float deltaTime);
    Vector3 UpdateRebound(float deltaTime);
    Vector3 UpdateEnd(float deltaTime);

private:
    Vector3 currentVelocity_;
    Vector3 ropeNormal_;

    float maxStretch_;
    float stretchResistance_;
    float reboundRate_;
    float delayTime_;

    float stretchDistance_;
    float currentDelay_;

    Phase phase_;
    bool isActive_ = false;

    std::function<Vector3(float)> updateFunc_;

public:
    bool IsActive() const;
    Phase GetPhase() const;
    float GetStretchRate() const;
};