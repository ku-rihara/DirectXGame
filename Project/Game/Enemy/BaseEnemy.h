#pragma once

//math
#include"Vector3.h"
//3d
#include"3d/ViewProjection.h"
//obj
#include"3d/Object3d.h"
#include"Colider/Collider.h"
class BaseEnemy :public Collider {
	static int instanceNum_;
private:
	std::unique_ptr<Object3d> objct3D_;
	WorldTransform transform_;
	int instanceID_;
	bool isDeath_;
public:

	/// ===================================================
	/// public  method
	/// ===================================================

	/// 初期化、更新、デバッグ、描画
	void Init();
	void Update();
	void Debug();
	void Draw(const ViewProjection& viewProjection);

	/// Collider
	Vector3 GetBaseCenterPosition() const override;
	void OnCollisionEnter([[maybe_unused]] Collider* other)override;

	/// ===================================================
	/// getter  method
	/// ===================================================
	Vector3 GetPos() { return transform_.translation_; }
	bool GetIsDeath() { return isDeath_; }


	/// ===================================================
	/// setter  method
	/// ===================================================
	void SetPos(const Vector3& pos) { transform_.translation_ = pos; }
};