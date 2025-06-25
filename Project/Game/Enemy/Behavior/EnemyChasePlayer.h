#pragma once

#include"BaseEnemyMoveBehavior.h"
#include"Easing.h"

class EnemyChasePlayer : public BaseEnemyMoveBehavior {
private:
	
	float chaseSpeedMax_;//ストーカースピード(速い)
	float chaseSpeedNormal_;	//ストーカースピード(遅い)

	//float waveAttackStartPos_;	//波攻撃の位置
	//float normalAttackStartPos_ ;//通常攻撃開始位置	

	//float attackCoolTime_;//攻撃クールタイム

	float distance_;

	bool isChase_;

	Easing<Vector2> spriteEasing_;
    Easing<Vector3> scaleEasing_;

	Vector2 tempSpriteScale_;
    Vector3 tempEnemyScale_;

public:
	//コンストラクタ
	EnemyChasePlayer(BaseEnemy* boss);
	~EnemyChasePlayer();


	void Update()override;
	void Debug()override;

};
