#include "PlayerMovement.h"
#include "3D/WorldTransform.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
// std
#include <algorithm>
#include <cmath>

void PlayerMovement::Init(const KetaEngine::ViewProjection* viewProjection) {
    viewProjection_ = viewProjection;
}

// =============================================================
// XZ 慣性移動
// =============================================================

void PlayerMovement::Update(const Vector3& rawInputDir, KetaEngine::WorldTransform& transform,
    float speed, float& outObjectiveAngle) {
    const float deltaTime = KetaEngine::Frame::DeltaTimeRate();

    // 目標速度: スティック入力をカメラ基準に変換
    Vector3 targetVelocity = Vector3::ZeroVector();
    if (rawInputDir.Length() > 0.001f) {
        Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_->rotation_.y);
        Vector3 inputDir       = TransformNormal(rawInputDir.Normalize(), rotateMatrix);
        targetVelocity         = inputDir * speed;
    }

    // Velocity更新
    UpdateVelocity(targetVelocity, deltaTime);

    // 座標に適用
    transform.translation_.x += velocity_.x * deltaTime;
    transform.translation_.z += velocity_.z * deltaTime;

    // 速度方向を向く
    if (GetSpeed() > 0.1f) {
        outObjectiveAngle = std::atan2(velocity_.x, velocity_.z);
    }
}

void PlayerMovement::UpdateVelocity(const Vector3& targetVelocity, float deltaTime) {

    // 入力されてるかの判定
    const bool hasInput = targetVelocity.Length() > 0.01f;

    if (!hasInput) {
        // 入力がない場合は徐々に速度を減衰させて停止させる
        const float frictionFactor = (std::max)(0.0f, 1.0f - deceleration_ * deltaTime);
        // 減衰後の速度を適用
        velocity_.x *= frictionFactor;
        velocity_.z *= frictionFactor;
        return;
    }

    // 方向転換時の急な反転を防ぐブレーキ処理
    if (velocity_.Length() > 0.01f) {
        // 90°以上逆方向ならブレーキ処理を入れる
        if (velocity_.Dot(targetVelocity) < 0.0f) {
            //逆方向入力中の減衰
            const float brakeFactor = (std::max)(0.0f, 1.0f - turnBrake_ * deltaTime);
            velocity_.x *= brakeFactor;
            velocity_.z *= brakeFactor;
        }
    }

    // 加速補間
    const float lerpRate = (std::min)(acceleration_ * deltaTime, 1.0f);
    velocity_ = Lerp(velocity_, targetVelocity, lerpRate);
}

float PlayerMovement::GetSpeed() const {
    return velocity_.Length();
}

void PlayerMovement::ForceStop() {
    velocity_ = Vector3::ZeroVector();
}

// =============================================================
// Y 軸移動
// =============================================================

void PlayerMovement::Jump(float& speed, float fallSpeedLimit, float gravity,
    KetaEngine::WorldTransform& transform, float groundY) {
    transform.translation_.y += speed * KetaEngine::Frame::DeltaTimeRate();
    Fall(speed, fallSpeedLimit, gravity, transform, groundY, true);
}

void PlayerMovement::Fall(float& speed, float fallSpeedLimit, float gravity,
    KetaEngine::WorldTransform& transform, float groundY, bool isJump) {
    if (!isJump) {
        transform.translation_.y += speed * KetaEngine::Frame::DeltaTimeRate();
    }

    // 重力加速
    speed = (std::max)(speed - gravity * KetaEngine::Frame::DeltaTimeRate(), -fallSpeedLimit);

    // 着地
    if (transform.translation_.y <= groundY) {
        speed                    = 0.0f;
        transform.translation_.y = groundY;
    }
}

// =============================================================
// フィールド境界制限
// =============================================================

void PlayerMovement::MoveToLimit(KetaEngine::WorldTransform& transform, const Vector3& fieldScale) {
    const Vector3 fieldCenter = Vector3::ZeroVector();

    //　移動範囲の計算
    const float radiusX = fieldScale.x - transform.scale_.x;
    const float radiusZ = fieldScale.z - transform.scale_.z;

    // フィールド内側かの判定
    const bool insideX = std::abs(transform.translation_.x - fieldCenter.x) <= radiusX;
    const bool insideZ = std::abs(transform.translation_.z - fieldCenter.z) <= radiusZ;

    // Xの押し戻し
    if (!insideX) {
        transform.translation_.x = std::clamp(
            transform.translation_.x,
            fieldCenter.x - radiusX,
            fieldCenter.x + radiusX);
    }

    // Zの押し戻し
    if (!insideZ) {
        transform.translation_.z = std::clamp(
            transform.translation_.z,
            fieldCenter.z - radiusZ,
            fieldCenter.z + radiusZ);
    }

    // 壁に当たったら速度もゼロ
    if (!insideX || !insideZ) {
        const Vector3 directionToCenter = (fieldCenter - transform.translation_).Normalize();
        transform.translation_.x += directionToCenter.x * 0.1f;
        transform.translation_.z += directionToCenter.z * 0.1f;
        ForceStop();
    }
}
