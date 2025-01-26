#pragma once

#include"Collider/OBBCollider.h"

class CollisionTest1 :public OBBCollider {
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