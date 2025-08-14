#pragma once

#include "BaseShakeState.h"


class ShakeWait : public BaseShakeState {
private:

	/// ===================================================
	///private varians
	/// ===================================================
	float speed_;
public:

	//コンストラクタ
	ShakeWait(GameCamera* camera);
	~ShakeWait();

	void Update()override;

	void Debug()override;

};
