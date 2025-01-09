#pragma once

#include "BaseGameCameraBehavior.h"
#include"Easing.h"

class GameCameraRoot : public BaseGameCameraBehavior {
private:
	float speed_;
	/// ===================================================
	///private varians
	/// ===================================================

public:

	//コンストラクタ
	GameCameraRoot(GameCamera* camera);
	~GameCameraRoot();

	void Update()override;

	void Debug()override;

};
