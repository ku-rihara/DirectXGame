#pragma once

#include "BaseGameCameraBehavior.h"
#include"EasingFunction.h"
#include "Easing.h"

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

	
	Easing outEase_;
	float waitTime_;
	float kWaitTime_;
	float timeOffset_;

	Easingw<Vector3> inEase_;
    Vector3 tempinOffset_;

public:

	//コンストラクタ
	GameCameraZoomInOut(GameCamera* camera);
	~GameCameraZoomInOut();

	void Update()override;
	void ShakeUpdate();

	void Debug()override;

};
