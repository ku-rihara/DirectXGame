#pragma once

#include"BaseAABBCollisionBox.h"

class PunchCollisionSecond :public BaseAABBCollisionBox {
private:
	
public:

	void Init()override;
	void Update()override;
	void Draw()override;


	Vector3 GetCollisionPos() const override;

	void SetZeroSizeCollision()override;
	void IsAdapt(bool is)override;
	void SetSize(const Vector3& size)override;
	void SetPosition(const Vector3& position)override;
	void SetOffset(const Vector3& offset)override;
};