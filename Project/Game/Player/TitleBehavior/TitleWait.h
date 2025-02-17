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
	Easing pressEase_;
	Easing restEase_;
	float  pressScale_;
	float waitTime_;

public:
	//コンストラクタ
	TitleWait(Player* boss);
	~TitleWait();

	void Update()override;
	void Debug()override;

};
