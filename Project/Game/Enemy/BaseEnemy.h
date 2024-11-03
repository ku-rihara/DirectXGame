#pragma once

//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
//obj
#include"3d/Object3d.h"
#include"Colider/AABBCollider.h"
class BaseEnemy :public AABBCollider {

protected:
	///============================================
	///protected variants
	/// ===========================================
	std::unique_ptr<Object3d> objct3D_;
	WorldTransform transform_;
	int instanceID_;
	bool isDeath_;
	int scoreValue_;
public:

	/// ===================================================
	/// public  method
	/// ===================================================

	/// 初期化、更新、デバッグ、描画
	virtual void Init(const std::string& instanceName, const std::string& extension);
	virtual void Update();
	virtual void Debug();
	virtual void Draw(const ViewProjection& viewProjection);

	/// Collider
	virtual	Vector3 GetBaseCenterPosition() const override;
	virtual	void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;

	/// ===================================================
	/// getter  method
	/// ===================================================
	Vector3 GetPos() { return transform_.translation_; }
	bool GetIsDeath() { return isDeath_; }
	int GetScoreValue()const { return scoreValue_; }

	/// ===================================================
	/// setter  method
	/// ===================================================
	void SetPos(const Vector3& pos) { transform_.translation_ = pos; }
};