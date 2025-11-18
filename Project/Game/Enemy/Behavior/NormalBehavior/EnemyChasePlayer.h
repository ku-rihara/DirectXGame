#pragma once

#include"BaseEnemyBehavior.h"
#include"Easing/Easing.h"

class EnemyChasePlayer : public BaseEnemyBehavior {
private:
	
	float chaseSpeedMax_;
	float chaseSpeedNormal_;

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
