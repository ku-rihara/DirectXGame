#pragma once

#include"BasePunchCollisionBox.h"

class PunchCollisionFirst :public BasePunchCollisionBox {
private:
	
public:

	void Init()override;
	void Update()override;
	void Draw()override;


	Vector3 GetCollisionPos() const override;
};