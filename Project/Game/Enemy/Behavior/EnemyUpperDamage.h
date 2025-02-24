#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing.h"

class EnemyUpperDamage : public BaseEnemyBehavior {
private:
	enum class Step {
		HITBACK,
		RETUNROOT,
	};
private:

	Step step_;
	float objectiveAngle_;
	Vector3 direction_;

	float speed_;
	float fallSpeedLimit_;
	float kFallSpeedLimit_;
	float gravity_;

	Easing easing_;

	float stopTime_;
	float kStopTime_;

public:
	//コンストラクタ
	EnemyUpperDamage(BaseEnemy* boss);
	~EnemyUpperDamage();


	void Update()override;
	void Debug()override;

};
