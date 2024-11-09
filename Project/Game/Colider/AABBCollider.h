#pragma once

#include "BaseCollider.h"
#include "Box.h"

class AABBCollider : public BaseCollider {
private:
	/// ===================================================
	/// private variant
	/// ===================================================
	Vector3 radiusVector_ = { 1.0f, 1.0f, 1.0f };  // AABBのサイズ
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

	virtual Vector3 GetBaseCenterPosition() const override;

	///=========================================================================================
	///  getter method
	///=========================================================================================

	/// AABBの範囲を取得
	AABB GetAABB() const { return aabb_; }
	Vector3 GetRadiusVector() const { return radiusVector_; }

	///=========================================================================================
   ///  setter method
   ///=========================================================================================
	void SetRadiusVector(Vector3 radius) { radiusVector_ = radius; }
};
