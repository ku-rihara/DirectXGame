#pragma once

#include"BaseComboAattackBehavior.h"
#include"EasingFunction.h"
#include"CollisionBox/AttackCollisionBox.h"
#include"utility/ParticleEditor/ParticleEmitter.h"
#include"Easing.h"

class RushAttack : public BaseComboAattackBehavior {
	enum class STEP {
		EMIT,
		RUSH,
		WAIT,
		RETURNROOT,
	};

public:
    // コンストラクタ
    RushAttack(Player* player);
    ~RushAttack();

    void Update() override;
    void Debug() override;

	void CollisionInit();
    void EasingInit();

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

	Vector3 targetRPos_;
	Vector3 targetLPos_;
	Vector3 rushTargetPos_;

	Easingw<Vector3> handRMoveEase_;
    Easingw<Vector3> handLMoveEase_;
    Easingw<Vector3> rushEase_;
    Vector3 tempRHandPos_;
    Vector3 tempLHandPos_;
    Vector3 tempRushPos_;

	std::unique_ptr<AttackCollisionBox>collisionBox_;
	std::unique_ptr<ParticleEmitter>emitter_;

	///wait
	float waitTime_;

};
