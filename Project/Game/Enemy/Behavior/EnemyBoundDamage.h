#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing.h"

class EnemyBoundDamage : public BaseEnemyBehaivor {
private:
	enum class Step {
		BOUND,
		RETUNROOT,
	};
private:

	Step step_;
	float objectiveAngle_;
	Vector3 direction_;

	float speed_;
	float fallSpeedLimit_;
	float gravity_;

	Easing easing_;

	float stopTime_;
	float kStopTime_;

	float rotate_;

public:
	//コンストラクタ
	EnemyBoundDamage(BaseEnemy* boss);
	~EnemyBoundDamage();


	void Update()override;
	void Debug()override;

};
