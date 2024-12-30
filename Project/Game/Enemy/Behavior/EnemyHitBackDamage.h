#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing.h"

class EnemyHitBackDamage : public BaseEnemyBehaivor {
private:
	enum class Step {
		DIRECTIONSET,
		HITBACK,
		RETUNROOT,
	};
private:

	Step step_;
	float objectiveAngle_;
	Vector3 initPos_;
	Vector3 backPos_;
	float speed_;
	Vector3 direction_;

	Easing easing_;

public:
	//コンストラクタ
	EnemyHitBackDamage(BaseEnemy* boss);
	~EnemyHitBackDamage();


	void Update()override;
	void Debug()override;

};
