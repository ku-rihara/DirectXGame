#include"AABB.h"
//当たり判定
bool IsCollision(const AABB& aabb, const Vector3& point) {
    return (point.x >= aabb.min.x && point.x <= aabb.max.x) &&
        (point.y >= aabb.min.y && point.y <= aabb.max.y) &&
        (point.z >= aabb.min.z && point.z <= aabb.max.z);
}