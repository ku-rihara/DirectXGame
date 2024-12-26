#pragma once

#include"Collider/AABBCollider.h"
#include"3d/WorldTransform.h"

class BasePunchCollisionBox :public AABBCollider {
private:
	bool isEnter_;
	bool isStay_;
	bool isExit_;
	WorldTransform transform_;
public:

	virtual void Init();
	virtual void Update();
	virtual void Draw();

	// 当たり
	 void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;
	 void OnCollisionStay([[maybe_unused]] BaseCollider* other)override;
	 void OnCollisionExit([[maybe_unused]] BaseCollider* other) override;

	virtual Vector3 GetCollisionPos() const override;
};