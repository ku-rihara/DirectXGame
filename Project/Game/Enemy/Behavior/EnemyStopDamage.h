#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing.h"

class EnemyStopDamage : public BaseEnemyBehaivor {
private:
	
private:

	

public:
	//コンストラクタ
	EnemyStopDamage(BaseEnemy* boss);
	~EnemyStopDamage();


	void Update()override;
	void Debug()override;

};
