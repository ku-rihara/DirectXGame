#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class ComboAttackSecond : public BaseComboAattackBehavior {
private:

	/// ===================================================
	///private varians
	/// ===================================================

public:
	//コンストラクタ
	ComboAttackSecond(Player* boss);
	~ComboAttackSecond();

	void Update()override;

	void Debug()override;

};
