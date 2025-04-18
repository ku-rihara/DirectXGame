#pragma once

#include"BaseEnemyMoveBehavior.h"
#include"Easing.h"

class EnemyRoot : public BaseEnemyMoveBehavior {
private:
	

	float distance_;

	bool isChase_;

	Easing spriteEase_;

public:
	//コンストラクタ
	EnemyRoot(BaseEnemy* boss);
	~EnemyRoot();


	void Update()override;
	void Debug()override;

};
