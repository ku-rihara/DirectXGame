#pragma once

#include "BaseGameCameraBehavior.h"
#include"EasingFunction.h"

class GameCameraRoot : public BaseGameCameraBehavior {
private:

	/// ===================================================
	///private varians
	/// ===================================================
	float speed_;
public:

	//コンストラクタ
	GameCameraRoot(GameCamera* camera);
	~GameCameraRoot();

	void Update()override;

	void Debug()override;

};
