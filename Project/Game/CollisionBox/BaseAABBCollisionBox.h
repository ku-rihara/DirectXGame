#pragma once

#include"Collider/AABBCollider.h"
#include"3d/WorldTransform.h"

class BaseAABBCollisionBox :public AABBCollider {
protected:
	bool isEnter_;
	bool isStay_;
	bool isExit_;
	WorldTransform transform_;
	Vector3 offset_;
public:

	virtual void Init();
	virtual void Update();
	virtual void Draw();

	virtual void SetZeroSizeCollision();
	virtual void IsAdapt(bool is);
	virtual void SetSize(const Vector3& size);
	virtual void SetPosition(const Vector3&position);
	virtual void SetOffset(const Vector3& offset);
	
	// 当たり
	 void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;
	 void OnCollisionStay([[maybe_unused]] BaseCollider* other)override;
	 void OnCollisionExit([[maybe_unused]] BaseCollider* other) override;

	virtual Vector3 GetCollisionPos() const override;
};