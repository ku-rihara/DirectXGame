#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing.h"

class EnemyThrustDamage : public BaseEnemyBehavior {
private:

	enum class Step {
		DIRECTIONSET,
		HITBACK,
		NEXTBEHAVIOR,
	};

private:

	Step step_;
	float objectiveAngle_;
	Vector3 initPos_;
	Vector3 backPos_;
	float speed_;
	Vector3 direction_;

	Easing<Vector3> easing_;
    Vector3 tempPos_;

	float hitStopTime_;
	float kHitStopTime_;

	/// 回転
	float rotate_;

public:
	//コンストラクタ
	EnemyThrustDamage(BaseEnemy* boss);
	~EnemyThrustDamage();


	void Update()override;
	void Debug()override;

};
