#pragma once

#include"BasePunchCollisionBox.h"

class PunchCollisionSecond :public BasePunchCollisionBox {
private:
	
public:

	void Init()override;
	void Update()override;
	void Draw()override;


	Vector3 GetCollisionPos() const override;
};