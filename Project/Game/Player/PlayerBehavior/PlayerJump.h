#pragma once

#include"BasePlayerBehavior.h"


class PlayerJump : public BasePlayerBehavior {
private:
	float speed_;
	/// ===================================================
	///private varians
	/// ===================================================

public:
	//コンストラクタ
    PlayerJump(Player* player);
	~PlayerJump();

	void Update()override;

	void Debug()override;

};
