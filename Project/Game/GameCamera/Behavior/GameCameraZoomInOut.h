#pragma once

#include "BaseGameCameraBehavior.h"
#include"Easing.h"

class GameCameraZoomInOut : public BaseGameCameraBehavior {
private:
	enum class Step {
		ZOOMIN,
		WAIT,
		ZOOMOUT,
		RETURNROOT,
	};
	/// ===================================================
	///private varians
	/// ===================================================
	Step step_;
	float shakeT_;
	float shakeTMax_;

	Easing inEase_;
	Easing outEase_;
	float waitTime_;
	float kWaitTime_;

public:

	//コンストラクタ
	GameCameraZoomInOut(GameCamera* camera);
	~GameCameraZoomInOut();

	void Update()override;
	void ShakeUpdate();

	void Debug()override;

};
