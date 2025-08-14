#pragma once

#include"BaseEnemyBehavior.h"


class EnemyDamageRoot : public BaseEnemyBehavior {
private:

private:

	

public:
	//コンストラクタ
	EnemyDamageRoot(BaseEnemy* boss);
	~EnemyDamageRoot();


	void Update()override;
	void Debug()override;

};
