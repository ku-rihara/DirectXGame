#pragma once

#include"BaseCollisionBox.h"

class PunchCollisionSecond :public BaseCollisionBox {
private:
	
public:

	void Init()override;
	void Update()override;
	void Draw()override;


	Vector3 GetCollisionPos() const override;
};