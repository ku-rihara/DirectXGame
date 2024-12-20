#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class ComboAttackFirst : public BaseComboAattackBehavior {
private:
	
	/// ===================================================
	///private varians
	/// ===================================================

public:
	//コンストラクタ
	ComboAttackFirst(Player* boss);
	~ComboAttackFirst();

	void Update()override;

	void Debug()override;

};
