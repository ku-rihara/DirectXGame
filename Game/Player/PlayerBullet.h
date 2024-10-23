#pragma once

#include"Object3d.h"
#include "WorldTransform.h"
//#include"Collider.h"
/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet {
private:
	static const int32_t kLifeTime = 60 * 5;//寿命
	int32_t deathTimer_ = kLifeTime;//デスタイマー
	bool isDeath_ = false; // デスフラグ
	//包含
	std::unique_ptr<Object3d>object3D_;
	Vector3 velocity_; // 速度
	uint32_t textureHandle_ = 0;

public:

	void Init(const Vector3& position, const Vector3& velocity);
	void Update();
	void Draw(const ViewProjection& viewProjection);

	void OnColligion();

	Vector3 GetWorldPos();
	bool GetIsDead() const { return isDeath_; }
};
