#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing.h"

class EnemyDeath : public BaseEnemyBehavior {
private:
	enum class Step {
		DIRECTIONSET,
		BLOW,
		BURST,
		WAIT,
		DEATH,
	};
private:

	Step step_;
	float rotate_;

	Vector3 savePos_;
	Vector3 backPos_;
	Vector3 preBlowPos_;
	float blowTime_;
	float blowJumpValue_;

	float burstTime_;
	Vector3 blowPower_;
	

	float gravity_;
	float kFallSpeedLimit_;
public:
	//コンストラクタ
	EnemyDeath(BaseEnemy* boss);
	~EnemyDeath();


	void Update()override;
	void Debug()override;

};
