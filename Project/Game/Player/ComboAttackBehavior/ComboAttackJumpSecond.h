#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class ComboAttackJumpSecond : public BaseComboAattackBehavior {
	enum class STEP {
		RUSH,
		WAIT,
		RETURNROOT,
	};
private:
	
	/// ===================================================
	/// private varians
	/// ===================================================
	STEP step_;

	/// rush
	Vector3 direction_;
	Vector3 initRHandPos_;
	Vector3 initLHandPos_;
	Vector3 initPos_;
	Easing  handMoveEasing_;
	float   rushEaseTime_;

	Vector3 targetRPos_;
	Vector3 targetLPos_;
	Vector3 rushTargetPos_;

	///wait
	float waitTime_;

public:
	//コンストラクタ
	ComboAttackJumpSecond(Player* boss);
	~ComboAttackJumpSecond();

	void Update()override;

	void Debug()override;

};
