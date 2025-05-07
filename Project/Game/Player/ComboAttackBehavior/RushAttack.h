#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"
#include"CollisionBox/AttackCollisionBox.h"
#include"utility/ParticleEditor/ParticleEmitter.h"

class RushAttack : public BaseComboAattackBehavior {
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

	std::unique_ptr<AttackCollisionBox>collisionBox_;
	std::unique_ptr<ParticleEmitter>emitter_;

	///wait
	float waitTime_;

public:
	//コンストラクタ
	RushAttack(Player* boss);
	~RushAttack();

	void Update()override;

	void Debug()override;

};
