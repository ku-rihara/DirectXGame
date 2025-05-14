#pragma once

#include"BaseEnemyMoveBehavior.h"
#include"Easing.h"

class EnemyWait : public BaseEnemyMoveBehavior {
private:
	

	float distance_;

	bool isChase_;

	Easing spriteEase_;

public:
	//コンストラクタ
	EnemyWait(BaseEnemy* boss);
	~EnemyWait();


	void Update()override;
	void Debug()override;

};
