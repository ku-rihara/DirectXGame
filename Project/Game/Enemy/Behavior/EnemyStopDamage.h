#pragma once

#include"BaseEnemyBehavior.h"


class EnemyStopDamage : public BaseEnemyBehavior {
private:
	
private:
	float stopTime_;
	float kStopTime_;
	float speed_;

public:
	//コンストラクタ
	EnemyStopDamage(BaseEnemy* boss);
	~EnemyStopDamage();


	void Update()override;
	void Debug()override;

};
