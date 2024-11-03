#include"Box.h"
//当たり判定
bool IsCollision(const AABB& aabb, const Vector3& point) {
    return (point.x >= aabb.min.x && point.x <= aabb.max.x) &&
        (point.y >= aabb.min.y && point.y <= aabb.max.y) &&
        (point.z >= aabb.min.z && point.z <= aabb.max.z);
}

// AABBとAABBの当たり判定
bool IsCollision(const AABB& aabbA, const AABB& aabbB) {
    // 各軸での判定
    return (aabbA.min.x <= aabbB.max.x && aabbA.max.x >= aabbB.min.x) &&
        (aabbA.min.y <= aabbB.max.y && aabbA.max.y >= aabbB.min.y) &&
        (aabbA.min.z <= aabbB.max.z && aabbA.max.z >= aabbB.min.z);
}