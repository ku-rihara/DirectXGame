#pragma once

#include"BaseEnemyBehavior.h"

#include"Easing/Easing.h"

class EnemyWait : public BaseEnemyBehavior {
private:
	

	float distance_;

	bool isChase_;

	Easing<Vector2> spriteEase_;
    Vector2 tempSpriteScale_;

public:
	//コンストラクタ
	EnemyWait(BaseEnemy* boss);
	~EnemyWait();


	void Update()override;
	void Debug()override;

};
