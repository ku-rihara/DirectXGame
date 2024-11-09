#pragma once

#include"3d/Object3d.h"
#include "3d/WorldTransform.h"
#include"2d/Sprite.h"
#include"Colider/OBBCollider.h"
/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBeam:public OBBCollider {
private:
	///============================================
	///private variants
	/// ===========================================
	WorldTransform transform_;
	WorldTransform transformR_;
	WorldTransform transformL_;
	Vector3 velocity_; // 速度
	

	/// gauge
	std::unique_ptr<Sprite>frameSprite_;
	std::unique_ptr<Sprite>gaugeSprite_;
	float gauge_;

	/// other class
	std::unique_ptr<Object3d>object3D_;
	std::unique_ptr<Object3d>object3DL_;

public:
	/// ===================================================
	/// public  method
	/// ===================================================

		/// 初期化、更新、可視化
	void Init();
	void Update(const Vector3& position, const Vector3& velocity);
	void Draw(const ViewProjection& viewProjection);
	void SpriteDraw();
	void Debug();

	void SetParent(const WorldTransform& worldTransform);

	/// gauge
	void DecreaseGauge();
	void IncreaseGauge();

	Vector3 PosSet(const Vector3& pos, const float& ofset);

	/// Collider
	Vector3 GetBaseCenterPosition() const override;
	Vector3 GetRotate() const override;
	
	/// ===================================================
    /// getter  methods
    /// ===================================================
	Vector3 GetWorldPos();
};
