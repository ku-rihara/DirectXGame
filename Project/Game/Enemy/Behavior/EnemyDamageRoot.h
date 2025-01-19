#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing.h"

class EnemyDamageRoot : public BaseEnemyBehaivor {
private:

private:

	

public:
	//コンストラクタ
	EnemyDamageRoot(BaseEnemy* boss);
	~EnemyDamageRoot();


	void Update()override;
	void Debug()override;

};
