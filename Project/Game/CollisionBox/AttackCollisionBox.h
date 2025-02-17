#pragma once

#include"BaseAABBCollisionBox.h"
#include<array>

class AttackCollisionBox :public BaseAABBCollisionBox {
public:
	enum class AttackType {
		NORMAL,
		FALL,
		RUSH,
		STOPPER,
		THRUST,
		UPPER,
		COUNT,
	};
private:
	bool isSlow_;
public:
	AttackType attackType_;
public:

	void Init()override;
	void Update()override;
	void Draw()override;

	void OnCollisionStay([[maybe_unused]] BaseCollider* other)override;

	 void IsAdapt(bool is)override;
	 void SetSize(const Vector3& size)override;
	 void SetPosition(const Vector3& position)override;
	 void SetOffset(const Vector3& offset)override;
	 void SetIsSlow(const bool& is) { isSlow_ = is; }

	 bool GetIsSlow()const { return isSlow_; }
	 Vector3 GetCollisionPos() const override;
	 AttackType GetTagList()const { return attackType_; }
};