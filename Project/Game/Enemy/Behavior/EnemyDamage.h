#pragma once

#include"BaseEnemyBehavior.h"

class EnemyDamage : public BaseEnemyBehaivor {
private:

	

public:
	//コンストラクタ
	EnemyDamage(BaseEnemy* boss);
	~EnemyDamage();


	void Update()override;
	void Debug()override;

};
