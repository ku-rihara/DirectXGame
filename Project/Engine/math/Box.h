#pragma once
#include"Vector3.h"
/// <summary>
/// 軸平行境界ボックス
/// </summary>
struct AABB {
	Vector3 min;
	Vector3 max;
};

/// <summary>
/// 有向境界ボックス
/// </summary>
struct OBB{
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};

/// <summary>
/// 球体による当たり判定形状
/// </summary>
struct Sphere {
	Vector3 center;
	float radius;
};


//当たり判定
bool IsCollision(const AABB& aabb, const Vector3& point);

// AABBとAABBの当たり判定
bool IsCollision(const AABB& aabbA, const AABB& aabbB);

bool IsCollision(const OBB& obb1, const OBB& obb2);

bool IsCollision(const OBB& obb, const AABB& aabb);

// Sphere同士の当たり判定
bool IsCollision(const Sphere& a, const Sphere& b);

// SphereとAABBの当たり判定
bool IsCollision(const Sphere& sphere, const AABB& aabb);

// SphereとOBBの当たり判定
bool IsCollision(const Sphere& sphere, const OBB& obb);