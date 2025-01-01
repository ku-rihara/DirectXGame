#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing.h"

class EnemyUpperDamage : public BaseEnemyBehaivor {
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
	float fallSpeedLimit_ = -1.2f;
	float gravity_;

	Easing easing_;

public:
	//コンストラクタ
	EnemyUpperDamage(BaseEnemy* boss);
	~EnemyUpperDamage();


	void Update()override;
	void Debug()override;

};
