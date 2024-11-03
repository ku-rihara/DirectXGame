#pragma once
#include"Vector3.h"
struct AABB {
	Vector3 min;
	Vector3 max;
};

struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};


//当たり判定
bool IsCollision(const AABB& aabb, const Vector3& point);

// AABBとAABBの当たり判定
bool IsCollision(const AABB& aabbA, const AABB& aabbB);

bool IsCollision(const OBB& obb1, const OBB& obb2);

bool IsCollision(const OBB& obb, const AABB& aabb);









