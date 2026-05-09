#include "CollisionPushUtils.h"
#include <algorithm>
#include <cmath>

void CollisionPushUtils::ApplySpherePush(
    Vector3& translation, const Vector3& otherPos,
    float myRadius, float otherRadius, float maxPush, float smoothing)
{
    Vector3 delta = translation - otherPos;
    delta.y       = 0.0f;

    float dist    = std::sqrt(delta.x * delta.x + delta.z * delta.z);
    float minDist = myRadius + otherRadius;

    if (dist < minDist) {
        Vector3 pushDirection;
        if (dist > 0.001f) {
            pushDirection = {delta.x / dist, 0.0f, delta.z / dist};
        } else {
            pushDirection = {1.0f, 0.0f, 0.0f};
        }
        // smoothing係数で1フレームの補正量を制限し、ガクガクを防ぐ
        float pushAmount = std::min(minDist - dist, maxPush) * smoothing;
        translation += pushDirection * pushAmount;
    }
}

void CollisionPushUtils::ApplyAABBSpherePush(
    Vector3& translation, const Vector3& otherPos,
    const Vector3& myScale, float otherRadius, float maxPush)
{
    Vector3 delta = translation - otherPos;

    float pushDistanceX = (otherRadius + myScale.x) / 2.0f + 0.1f;
    float pushAmountX   = std::max(0.0f, pushDistanceX - std::abs(delta.x));
    float pushAmountZ   = std::max(0.0f, pushDistanceX - std::abs(delta.z));

    Vector3 pushDirection = {0.0f, 0.0f, 0.0f};
    float   pushDistance  = 0.0f;

    if (pushAmountX > 0.0f && pushAmountZ > 0.0f) {
        if (pushAmountX > pushAmountZ) {
            pushDistance  = pushAmountX;
            pushDirection = {delta.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f};
        } else {
            pushDistance  = pushAmountZ;
            pushDirection = {0.0f, 0.0f, delta.z > 0 ? 1.0f : -1.0f};
        }
    } else if (pushAmountX > 0.0f) {
        pushDistance  = pushAmountX;
        pushDirection = {delta.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f};
    } else if (pushAmountZ > 0.0f) {
        pushDistance  = pushAmountZ;
        pushDirection = {0.0f, 0.0f, delta.z > 0 ? 1.0f : -1.0f};
    }

    if (pushDistance > 0.0f) {
        translation += pushDirection * std::min(pushDistance, maxPush);
    }
}
