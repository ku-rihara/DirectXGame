#pragma once

#include"BaseCollisionBox.h"

class PunchCollisionBox :public BaseCollisionBox {
private:
	
public:

	void Init()override;
	void Update()override;
	void Draw()override;


	Vector3 GetCollisionPos() const override;
};