#pragma once

#include "BaseShakeState.h"
#include"Easing.h"

class ShakeStateRoot : public BaseShakeState {
private:

	/// ===================================================
	///private varians
	/// ===================================================
	float speed_;
public:

	//コンストラクタ
	ShakeStateRoot(GameCamera* camera);
	~ShakeStateRoot();

	void Update()override;

	void Debug()override;

};
