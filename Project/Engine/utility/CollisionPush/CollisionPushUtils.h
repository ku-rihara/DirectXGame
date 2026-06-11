#pragma once
#include "MathFunction.h"

// 押し戻し計算ユーティリティ
namespace CollisionPushUtils {

    // 球-球 押し戻し
    void ApplySpherePush(Vector3& translation, const Vector3& otherPos,
                         float myRadius, float otherRadius,
                         float maxPush  = 1.0f,
                         float smoothing = 0.3f);

    // AABB-球 押し戻し
    void ApplyAABBSpherePush(Vector3& translation, const Vector3& otherPos,
                             const Vector3& myScale, float otherRadius,
                             float maxPush = 0.5f);

} // namespace CollisionPushUtils
