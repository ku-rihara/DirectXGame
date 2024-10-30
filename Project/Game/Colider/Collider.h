#pragma once
#include "Vector3.h"
#include "3d/WorldTransform.h"
#include "3d/Object3d.h"
#include "AABB.h"

class Collider {
private:
	/// ===================================================
	/// メンバ変数
	/// ===================================================
	Vector3 radiusVector_ = { 1.5f, 1.5f, 1.5f };
	WorldTransform transform_;
	uint32_t typeID_ = 0u;
	bool isColliding_;
	// デバッグ表示用モデル
	std::unique_ptr<Object3d> object3d_;

	// AABB範囲
	AABB aabb_;

public:

	Collider();
	 ~Collider();

	/// ===================================================
	/// public  method
	/// ===================================================

		/// 初期化、更新、可視化
	void Init();
	void UpdateWorldTransform();
	void Draw( const ViewProjection& viewProjection);

	/// 中心座標取得
	virtual Vector3 GetBaseCenterPosition() const = 0;

	/// 各当たり判定
	virtual void OnCollisionEnter([[maybe_unused]] Collider* other) {}
	virtual void OnCollisionStay([[maybe_unused]] Collider* other) {}
	virtual void OnCollisionExit([[maybe_unused]] Collider* other) {}

   /// ===================================================
   /// getter  methods
   /// ===================================================
	Vector3 GetRadiusVector() const { return radiusVector_; }
	uint32_t GetTypeID() const { return typeID_; }
	bool GetIsColliding() const { return isColliding_; }
	AABB GetAABB() const { return aabb_; }

	/// ===================================================
   /// setter  methods
   /// ===================================================
	void SetRadiusVector(Vector3 radius) { radiusVector_ = radius; }
	void SetTypeID(uint32_t typeID) { typeID_ = typeID; }
	void SetColliding(bool colliding) { isColliding_ = colliding; }
};
