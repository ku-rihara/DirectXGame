#pragma once

//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
//obj
#include"3d/Object3d.h"
#include"Colider/AABBCollider.h"

class BaseEnemy :public AABBCollider {
private:
	Vector3 target_;
protected:
	///============================================
	///protected variants
	/// ===========================================
	std::unique_ptr<Object3d> objct3D_;
	WorldTransform transform_;

	int instanceID_;
	bool isDeath_;
	int scoreValue_;
	float distanceMax_;
	bool isMove_;
	/// 出現
	float easeT_;
	const float easeTMax_ = 0.2f;
	int hp_;
	float initialX_;

public:

	/// ===================================================
	/// public  method
	/// ===================================================

	/// 初期化、更新、デバッグ、描画
	virtual void Init(const std::string& instanceName, const std::string& extension);
	virtual void Update();
	virtual void Debug();
	virtual void Draw(const ViewProjection& viewProjection);

	void SetTarget(const Vector3&pos);

	virtual void Move() = 0;

	/// Collider
	virtual	Vector3 GetBaseCenterPosition() const override;
	virtual	void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;

	/// ===================================================
	/// getter  method
	/// ===================================================
	Vector3 GetPos() { return transform_.translation_; }
	bool GetIsDeath() { return isDeath_; }
	int GetScoreValue()const { return scoreValue_; }
	Vector3 GetTargetPos()const { return target_; }

	Vector3 GetWorldPos();
	/// ===================================================
	/// setter  method
	/// ===================================================
	void SetPos(const Vector3& pos);
};