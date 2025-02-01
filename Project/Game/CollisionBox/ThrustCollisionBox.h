#pragma once

#include"BaseAABBCollisionBox.h"

class ThrustCollisionBox :public BaseAABBCollisionBox {
private:

	 bool isSlow_;
public:

	void Init()override;
	void Update()override;
	void Draw()override;

	Vector3 GetCollisionPos() const override;
	void IsAdapt(bool is)override;
	void SetSize(const Vector3& size)override;
	void SetPosition(const Vector3& position)override;
	void SetOffset(const Vector3& offset)override;
	
	void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;

	bool GetIsSlow()const { return isSlow_; }
	void SetIsSlow(const bool& is) { isSlow_ = is; }
	
};