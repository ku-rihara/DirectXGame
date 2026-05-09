#pragma once
#include "MathFunction.h"

// 押し戻し計算ユーティリティ
namespace CollisionPushUtils {

    /// @brief 球-球 押し戻し
    /// @param translation    押し戻す対象の座標
    /// @param otherPos       相手の中心座標
    /// @param myRadius       自分のコリジョン半径
    /// @param otherRadius    相手のコリジョン半径
    /// @param maxPush        ワープ防止用の最大移動量
    /// @param smoothing      補間係数 0〜1
    void ApplySpherePush(Vector3& translation, const Vector3& otherPos,
                         float myRadius, float otherRadius,
                         float maxPush  = 1.0f,
                         float smoothing = 0.3f);

    /// @brief AABB-球 押し戻し
    /// @param translation    押し戻す対象の座標
    /// @param otherPos       相手（球）の中心座標
    /// @param myScale        自分（AABB）のスケール
    /// @param otherRadius    相手のコリジョン半径
    /// @param maxPush        ワープ防止用の最大移動量
    void ApplyAABBSpherePush(Vector3& translation, const Vector3& otherPos,
                             const Vector3& myScale, float otherRadius,
                             float maxPush = 0.5f);

} // namespace CollisionPushUtils
