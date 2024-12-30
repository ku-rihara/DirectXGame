#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class ComboAttackRoot : public BaseComboAattackBehavior {

private:
	enum class AttackPatern {
		NORMAL,
		JUMP,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	AttackPatern attackPatern_;

public:
	//コンストラクタ
	ComboAttackRoot(Player* boss);
	~ComboAttackRoot();

	void Update()override;
	void Debug()override;

	void JudgeAttackPatern(); //攻撃パターンの判断　

	void NormalAttackChangeForJoyStick();
	void JumpAttackAttackChangeForJoyStick();
};
