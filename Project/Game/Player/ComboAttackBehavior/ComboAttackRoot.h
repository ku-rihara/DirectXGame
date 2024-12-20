#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class ComboAttackRoot : public BaseComboAattackBehavior {
private:

	/// ===================================================
	///private varians
	/// ===================================================

public:
	//コンストラクタ
	ComboAttackRoot(Player* boss);
	~ComboAttackRoot();

	void Update()override;

	void Debug()override;

};
