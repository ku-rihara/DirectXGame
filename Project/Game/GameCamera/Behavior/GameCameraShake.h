#pragma once

#include "BaseGameCameraBehavior.h"
#include"Easing.h"

class GameCameraShake : public BaseGameCameraBehavior {
private:
	
	/// ===================================================
	///private varians
	/// ===================================================

	float shakeT_;
	float shakeTMax_;

public:

	//コンストラクタ
	GameCameraShake(GameCamera* camera);
	~GameCameraShake();

	void Update()override;

	void Debug()override;

};
