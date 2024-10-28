#pragma once
#include"Vector3.h"
struct AABB {
	Vector3 min;
	Vector3 max;
};

//当たり判定
bool IsCollision(const AABB& aabb, const Vector3& point);