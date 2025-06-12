#pragma once

#include "BaseGameCameraBehavior.h"
#include"EasingFunction.h"

class GameCameraBackLash : public BaseGameCameraBehavior {
private:
	enum class Step {
		OUTIN,
		RETURNROOT,
	};
	/// ===================================================
	///private varians
	/// ===================================================
	Step step_;
	Easing ease_;
public:

	//コンストラクタ
	GameCameraBackLash(GameCamera* camera);
	~GameCameraBackLash();

	void Update()override;

	void Debug()override;

};
