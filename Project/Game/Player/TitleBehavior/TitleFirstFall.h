#pragma once

#include"BaseTitleBehavior.h"
#include"Easing.h"

class TitleFirstFall : public BaseTitleBehavior {
public:
    // コンストラクタ
    TitleFirstFall(Player* player);
    ~TitleFirstFall();

    void Update() override;
    void Debug() override;
    void EasingInit();

private:
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

	Easing<Vector3> landScaleEasing_;
    Vector3 tempScale_;
    Easing<float> fallEase_;
    float tempPosY_;

	//float fallSpeed_;
	float waitTime_;



};
