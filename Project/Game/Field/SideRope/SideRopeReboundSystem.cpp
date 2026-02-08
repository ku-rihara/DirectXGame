#include "SideRopeReboundSystem.h"
#include "Frame/Frame.h"

void SideRopeReboundSystem::Init(
    const Vector3& initialVelocity,
    const Vector3& ropeNormal,
    const RopeBoundParam& ropeParam) {

    currentVelocity_ = initialVelocity;
    ropeNormal_      = ropeNormal.Normalize();
    ropeParam_       = ropeParam;

    stretchDistance_ = 0.0f;
    currentDelay_    = 0.0f;
    reboundDistance_ = 0.0f;
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

    // ロープ方向以外の速度成分を保持
    Vector3 tangentialVelocity = currentVelocity_ - (ropeNormal_ * velocityAlongRope);

    // 今フレームで伸びる量を計算
    float stretchThisFrame = velocityAlongRope * deltaTime;
    stretchDistance_ += stretchThisFrame;

    // バネのような引き戻し力を計算
    float stretchRatio  = stretchDistance_ / ropeParam_.maxStretch;
    float pullBackForce = stretchRatio * ropeParam_.stretchResistance;

    // 速度を減速
    velocityAlongRope -= pullBackForce * deltaTime;

    // 最大まで伸びたかチェック
    if (stretchDistance_ >= ropeParam_.maxStretch) {
        // 伸びすぎた分を補正
        float overshoot  = stretchDistance_ - ropeParam_.maxStretch;
        stretchDistance_ = ropeParam_.maxStretch;
        stretchThisFrame -= overshoot;

        // 反発フェーズへ移行
        currentDelay_ = 0.0f;
        phase_        = Phase::REBOUND;

        updateFunc_ = [this](float deltaTime) {
            return UpdateRebound(deltaTime);
        };

        // 最大まで伸びた時点での移動量を返す
        return ropeNormal_ * stretchThisFrame + tangentialVelocity * deltaTime;
    }

    // 速度が0または負になったら反発フェーズへ
    if (velocityAlongRope <= 0.0f) {
        currentDelay_ = 0.0f;
        phase_        = Phase::REBOUND;

        updateFunc_ = [this](float deltaTime) {
            return UpdateRebound(deltaTime);
        };

        return ropeNormal_ * stretchThisFrame + tangentialVelocity * deltaTime;
    }

    // 速度を更新
    currentVelocity_ = ropeNormal_ * velocityAlongRope + tangentialVelocity;

    // 今フレームでの移動量を返す
    return ropeNormal_ * stretchThisFrame + tangentialVelocity * deltaTime;
}

Vector3 SideRopeReboundSystem::UpdateRebound(float deltaTime) {

    // 遅延処理
    if (currentDelay_ < ropeParam_.delayTime) {
        currentDelay_ += deltaTime;
        return Vector3::ZeroVector();
    }

    // 伸び率が高いほど強く跳ね返る
    float stretchRatio = stretchDistance_ / ropeParam_.maxStretch;
    float reboundSpeed = stretchRatio * ropeParam_.reboundRate;

    // 反発方向は伸びた方向の逆
    Vector3 reboundVelocity = -ropeNormal_ * reboundSpeed;

    // 今フレームの移動量を計算
    float reboundThisFrame = reboundSpeed * deltaTime;
    reboundDistance_ += reboundThisFrame;

    // 伸びた距離まで戻ったら終了
    if (reboundDistance_ >= stretchDistance_) {
        float overshoot = reboundDistance_ - stretchDistance_;
        reboundThisFrame -= overshoot;

        // 終了フェーズへ移行
        phase_      = Phase::END;
        updateFunc_ = [this](float deltaTime) {
            return UpdateEnd(deltaTime);
        };

        return -ropeNormal_ * reboundThisFrame;
    }

    // 跳ね返りの移動量を返す
    return reboundVelocity * deltaTime;
}

Vector3 SideRopeReboundSystem::UpdateEnd(float deltaTime) {
    (void)deltaTime;
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