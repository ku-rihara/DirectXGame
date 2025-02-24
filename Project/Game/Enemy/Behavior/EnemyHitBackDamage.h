#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing.h"

class EnemyHitBackDamage : public BaseEnemyBehavior {
private:
	enum class Step {
		DIRECTIONSET,
		HITBACK,
		RETUNROOT,
	};
private:

	Step step_;
	
	Vector3 initPos_;
	Vector3 backPos_;
	float speed_;
	
	//hitbackmove
	Easing archingbackEase_;
	float preRotate_;

	Easing easing_;

public:
	//コンストラクタ
	EnemyHitBackDamage(BaseEnemy* boss);
	~EnemyHitBackDamage();


	void Update()override;
	void Debug()override;

};
