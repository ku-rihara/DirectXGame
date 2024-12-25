#pragma once

#include"Collider/AABBCollider.h"
#include"3d/WorldTransform.h"

class CollisionTest2 :public AABBCollider {
private:

	bool isEnter_;
	bool isStay_;
	bool isExit_;
	WorldTransform transform_;
public:

	///　Init,Update,Draw
	void Init();
	void Update();
	void Draw();

	// 当たり判定のコールバック関数
	void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;
	void OnCollisionStay([[maybe_unused]] BaseCollider* other)override;
	void OnCollisionExit([[maybe_unused]] BaseCollider* other) override;

	 Vector3 GetCollisionPos() const override;
};