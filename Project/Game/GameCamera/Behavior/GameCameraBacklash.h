#pragma once

#include "BaseGameCameraBehavior.h"

#include"Easing/Easing.h"

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
	Easing<Vector3> ease_;
    Vector3 tempOffset_;

public:

	//コンストラクタ
	GameCameraBackLash(GameCamera* camera);
	~GameCameraBackLash();

	void Update()override;

	void Debug()override;

};
