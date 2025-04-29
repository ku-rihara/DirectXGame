#pragma once

#include"Core/KTFramework.h"

class KTGame :public KTFramework {


	///======================================
    ///public method
    ///======================================
public:
	 void Init()override;
	 void Update()override;
	 void Draw()override;
	 void DarwOffscreen()override;
	 void Finalize()override;
};