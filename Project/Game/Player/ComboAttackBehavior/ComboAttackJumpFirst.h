#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class ComboAttackJumpFirst : public BaseComboAattackBehavior {
private:
	
	/// ===================================================
	///private varians
	/// ===================================================

public:
	//コンストラクタ
	ComboAttackJumpFirst(Player* boss);
	~ComboAttackJumpFirst();

	void Update()override;

	void Debug()override;

};
