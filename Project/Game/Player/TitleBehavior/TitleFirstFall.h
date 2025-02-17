#pragma once

#include"BaseTitleBehavior.h"
#include"Easing.h"

class TitleFirstFall : public BaseTitleBehavior {

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

	float fallSpeed_;

	float waitTime_;

public:
	//コンストラクタ
	TitleFirstFall(Player* boss);
	~TitleFirstFall();

	void Update()override;

	void Debug()override;

};
