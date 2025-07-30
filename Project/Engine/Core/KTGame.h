#pragma once

#include"Core/KTFramework.h"

class KTGame :public KTFramework {


	
public:

	KTGame() = default;
    ~KTGame() = default;

	///======================================
    /// public method
    ///======================================
	 void Init()override;
	 void Update()override;
	 void Draw()override;
	 void DrawPostEffect()override;
     void DrawShadow() override; 
	 void Finalize()override;
};