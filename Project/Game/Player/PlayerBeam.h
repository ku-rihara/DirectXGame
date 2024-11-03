#pragma once

#include"3d/Object3d.h"
#include "3d/WorldTransform.h"
#include"Colider/OBBCollider.h"
/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBeam:public OBBCollider {
private:
	///============================================
	///private variants
	/// ===========================================
	static const int32_t kLifeTime = 60 * 5;//寿命
	int32_t deathTimer_ = kLifeTime;//デスタイマー
	bool isDeath_ = false; // デスフラグ
	WorldTransform transform_;
	Vector3 velocity_; // 速度
	uint32_t textureHandle_ = 0;

	/// other class
	std::unique_ptr<Object3d>object3D_;
	

public:
	/// ===================================================
	/// public  method
	/// ===================================================

		/// 初期化、更新、可視化
	void Init();
	void Update(const Vector3& position, const Vector3& velocity);
	void Draw(const ViewProjection& viewProjection);

	/// Collider
	Vector3 GetBaseCenterPosition() const override;
	Vector3 GetRotate() const override;
	//void OnCollisionEnter([[maybe_unused]] Colider* other)override;

	/// ===================================================
    /// getter  methods
    /// ===================================================
	Vector3 GetWorldPos();
	bool GetIsDead() const { return isDeath_; }
};
