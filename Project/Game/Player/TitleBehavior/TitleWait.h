#pragma once

#include"BaseTitleBehavior.h"
#include"Easing.h"

class TitleWait : public BaseTitleBehavior {

	enum class STEP {
		PRESS,
		RESTORATIVE,
	};
	
private:
	
	/// ===================================================
	///private varians
	/// ===================================================
	STEP  step_;
	Easing<float> pressEase_;
    float tempScaleY_;
	float  pressScale_;
	float waitTime_;

public:
	//コンストラクタ
    TitleWait(Player* player);
	~TitleWait();

	void Update()override;
	void Debug()override;

};
