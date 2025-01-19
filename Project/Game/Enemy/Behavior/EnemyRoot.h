#pragma once

#include"BaseEnemyMoveBehavior.h"
#include"Easing.h"

class EnemyRoot : public BaseEnemyMoveBehavior {
private:
	
	float chaseSpeedMax_;//ストーカースピード(速い)
	float chaseSpeedNormal_;	//ストーカースピード(遅い)

	float waveAttackStartPos_;	//波攻撃の位置
	float normalAttackStartPos_ ;//通常攻撃開始位置	

	float attackCoolTime_;//攻撃クールタイム

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
