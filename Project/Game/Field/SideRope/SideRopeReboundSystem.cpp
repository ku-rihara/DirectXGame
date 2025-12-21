#include "SideRopeReboundSystem.h"
#include "Frame/Frame.h"

void SideRopeReboundSystem::Init(
    const Vector3& initialVelocity,
    const Vector3& ropeNormal,
    const BounceParam& ropeParam) {

    currentVelocity_   = initialVelocity;
    ropeNormal_        = ropeNormal.Normalize();
    ropeParam_         = ropeParam;

    stretchDistance_ = 0.0f;
    currentDelay_    = 0.0f;
    phase_           = Phase::STRETCH;
    isActive_        = true;

    updateFunc_ = [this](float deltaTime) {
        return UpdateStretch(deltaTime);
    };
}

Vector3 SideRopeReboundSystem::Update(float deltaTime) {

    if (!isActive_) {
        return Vector3::ZeroVector();
    }

    return updateFunc_(deltaTime);
}

Vector3 SideRopeReboundSystem::UpdateStretch(float deltaTime) {

    // ロープ方向への速度成分を取得
    float velocityAlongRope = currentVelocity_.Dot(ropeNormal_);

    // 抵抗による減速
    velocityAlongRope -= ropeParam_.stretchResistance * deltaTime;

    // 伸びる量を累積
    stretchDistance_ += velocityAlongRope * deltaTime;

    // 最大まで伸びたら反発するフェーズへ移行
    if (stretchDistance_ >= ropeParam_.maxStretch || velocityAlongRope <= 0.0f) {
        stretchDistance_ = ropeParam_.maxStretch;
        currentDelay_    = 0.0f;
        phase_           = Phase::REBOUND;

        updateFunc_ = [this](float deltaTime) {
            return UpdateRebound(deltaTime);
        };

        return Vector3::ZeroVector();
    }

    currentVelocity_ = ropeNormal_ * velocityAlongRope;
    return currentVelocity_;
}

Vector3 SideRopeReboundSystem::UpdateRebound(float deltaTime) {

    // 遅延処理
    if (currentDelay_ < ropeParam_.delayTime) {
        currentDelay_ += deltaTime;
        return Vector3::ZeroVector();
    }

    // 反発速度を計算
    float reboundSpeed      = (stretchDistance_ / ropeParam_.maxStretch) * ropeParam_.reboundRate;
    Vector3 reboundVelocity = -ropeNormal_ * reboundSpeed;

    phase_ = Phase::END;

    updateFunc_ = [this](float deltaTime) {
        return UpdateEnd(deltaTime);
    };

    return reboundVelocity;
}

Vector3 SideRopeReboundSystem::UpdateEnd(float deltaTime) {
    deltaTime;
    isActive_ = false;
    return Vector3::ZeroVector();
}

bool SideRopeReboundSystem::IsActive() const {
    return isActive_;
}

SideRopeReboundSystem::Phase SideRopeReboundSystem::GetPhase() const {
    return phase_;
}

float SideRopeReboundSystem::GetStretchRate() const {
    return stretchDistance_ / ropeParam_.maxStretch;
}