#pragma once

#include "BaseShakeState.h"
#include"EasingFunction.h"

class StateShaking: public BaseShakeState {
private:
	enum class Step {
		SHAKE,
		RETURNROOT,
	};
	/// ===================================================
	///private varians
	/// ===================================================
	Step step_;
	float shakeT_;
	float shakeTMax_;
	//Easing shakeEase_;

public:

	//コンストラクタ
	StateShaking(GameCamera* camera);
	~StateShaking();

	void Update()override;

	void Debug()override;

};
