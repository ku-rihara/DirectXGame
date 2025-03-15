#pragma once

#include"Core/KTFramework.h"

class KTGame :public KTFramework {


	///======================================
    ///public method
    ///======================================
public:
	 void Init()override;
	 void Update()override;
	 void DrawModel()override;
	 void DrawSprite()override;
	 void Finalize()override;
};