#pragma once

#include "BaseCollider.h"
#include "Box.h"

class AABBCollider : public BaseCollider {
private:
	/// ===================================================
	/// private variant
	/// ===================================================
	Vector3 collisionScale_ = { 1.5f, 1.5f, 1.5f };  // AABBのサイズ
	AABB aabb_;  // AABB範囲

public:
	AABBCollider();
	~AABBCollider();
	/// ===================================================
	/// public  method
	/// ===================================================

	void Init() override;
	void UpdateWorldTransform() override;
	void DrawDebugCube(const ViewProjection& viewProjection)override;

	virtual Vector3 GetCollisionPos() const override;

	///=========================================================================================
	///  getter method
	///=========================================================================================

	/// AABBの範囲を取得
	AABB GetAABB() const { return aabb_; }
	Vector3 GetCollisonScale() const { return collisionScale_; }

	///=========================================================================================
   ///  setter method
   ///=========================================================================================
	void SetCollisionScale(Vector3 radius) { collisionScale_ = radius; }
};
