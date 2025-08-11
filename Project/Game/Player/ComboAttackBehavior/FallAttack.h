#pragma once

#include"BaseComboAattackBehavior.h"
#include"CollisionBox/PlayerAttackController.h"
#include"EasingFunction.h"
#include"Easing.h"

class FallAttack : public BaseComboAattackBehavior {
public:
    // コンストラクタ
    FallAttack(Player* player);
    ~FallAttack();

    void Update() override;
    void Debug() override;

	void CollisionInit();
    void EasingInit();

private:
	enum class STEP {
		FALL,
        FALLFINISH,
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
	/*float fallEaseT_;*/
	float fallRotateY_;

	///landing
	float boundSpeed_;
	Vector3 initRotate_;
	float boundFallSpeedLimit_;
	float landRotateX_;
	float rotateXSpeed_;
	float rotateYSpeed_;
	float gravity_;

	float tempWorldPosY_;
    Easing<float> fallEase_;

	Vector3 tempLandScale_;
	Easing<Vector3> landScaleEasing_;
	float waitTime_;
};
