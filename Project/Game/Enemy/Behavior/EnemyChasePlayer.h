#pragma once

#include"BaseEnemyBehavior.h"

class EnemyChasePlayer : public BaseEnemyBehaivor {
private:
	
	float chaseSpeedMax_;//ストーカースピード(速い)
	float chaseSpeedNormal_;	//ストーカースピード(遅い)

	float waveAttackStartPos_;	//波攻撃の位置
	float normalAttackStartPos_ ;//通常攻撃開始位置	

	float attackCoolTime_;//攻撃クールタイム

	float distance_;

	bool isChase_;

public:
	//コンストラクタ
	EnemyChasePlayer(BaseEnemy* boss);
	~EnemyChasePlayer();


	void Update()override;
	void Debug()override;

};
