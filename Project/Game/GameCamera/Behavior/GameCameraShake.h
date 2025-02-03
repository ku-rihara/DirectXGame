#pragma once

#include "BaseGameCameraBehavior.h"
#include"Easing.h"

class GameCameraShake : public BaseGameCameraBehavior {
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
	Easing shakeEase_;

public:

	//コンストラクタ
	GameCameraShake(GameCamera* camera);
	~GameCameraShake();

	void Update()override;

	void Debug()override;

};
