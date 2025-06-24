#pragma once

#include "BaseGameCameraBehavior.h"
#include"EasingFunction.h"
#include "Easing.h"

class GameCameraZoomInOut : public BaseGameCameraBehavior {
private:
	enum class Step {
		ZOOMIN,
		ZOOMOUT,
		RETURNROOT,
	};
	/// ===================================================
	///private varians
	/// ===================================================
	Step step_;
	float shakeT_;
	float shakeTMax_;


	float waitTime_;
	float kWaitTime_;
	float timeOffset_;

	Easing<Vector3> inEase_;
    Easing<Vector3> outEase_;
    Vector3 tempinOffset_;
  /*  Vector3 tempOutOffset_;*/

public:

	//コンストラクタ
	GameCameraZoomInOut(GameCamera* camera);
	~GameCameraZoomInOut();

	void Update()override;
	void ShakeUpdate();

	void Debug()override;

};
