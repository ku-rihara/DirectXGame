#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class ComboAttackJumpFirst : public BaseComboAattackBehavior {

	enum class STEP {
		FALL,
		LANDING,
		WAIT,
		RETURNROOT,
	};
	
private:
	
	/// ===================================================
	///private varians
	/// ===================================================
	STEP step_;

	/// fall
	float fallInitPosLHand_;
	float fallInitPosRHand_;
	float playerInitPosY_;
	float fallEaseT_;
	float fallRotateY_;

	///landing
	float boundSpeed_;
	Vector3 initRotate_;
	float boundFallSpeedLimit_;
	float landRotateX_;
	float rotateXSpeed_;
	float rotateYSpeed_;
	float gravity_;
	Easing landScaleEasing_;

	float waitTime_;

public:
	//コンストラクタ
	ComboAttackJumpFirst(Player* boss);
	~ComboAttackJumpFirst();

	void Update()override;

	void Debug()override;

};
