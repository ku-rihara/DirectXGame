#pragma once

#include"Collider/AABBCollider.h"
#include"3d/WorldTransform.h"

class CollisionTest3 :public AABBCollider {
private:
	bool isEnter_;
	bool isStay_;
	bool isExit_;
	WorldTransform transform_;
public:
	void Init();
	void Update();
	void Draw();

	// 当たり
	void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;
	void OnCollisionStay([[maybe_unused]] BaseCollider* other)override;
	void OnCollisionExit([[maybe_unused]] BaseCollider* other) override;

	 Vector3 GetCollisionPos() const override;
};