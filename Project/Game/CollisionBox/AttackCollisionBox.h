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
	bool isHitStop_;
public:
	AttackType attackType_;
public:

	void Init()override;
	void Update()override;
	void Draw()override;

	void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;
	void OnCollisionStay([[maybe_unused]] BaseCollider* other)override;

	 void IsAdapt(bool is)override;
	 void SetSize(const Vector3& size)override;
	 void SetPosition(const Vector3& position)override;
	 void SetOffset(const Vector3& offset)override;

	 void SetIsSlow(const bool& is) { isSlow_ = is; }
	 void SetIsHitStop(const bool& is) { isHitStop_ = is; }

	 Vector3 GetCollisionPos() const override;
	 bool GetIsSlow()const { return isSlow_; }
	 bool GetIsHitStop()const { return isHitStop_; }
	 AttackType GetTagList()const { return attackType_; }
};