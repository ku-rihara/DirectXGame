#pragma once

#include"BasePlayerBehavior.h"
#include"Easing.h"

class PlayerRoot : public BasePlayerBehavior {
private:

/// ===================================================
///private varians
/// ===================================================

	
public:
	//コンストラクタ
	PlayerRoot(Player* boss);
	~PlayerRoot();

	void Update()override;
	void Debug()override;

	void JumpForJoyState();

};
