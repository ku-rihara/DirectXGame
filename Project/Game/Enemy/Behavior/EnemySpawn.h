#pragma once

#include"BaseEnemyMoveBehavior.h"
#include"Easing.h"

class EnemySpawn : public BaseEnemyMoveBehavior {
private:
	enum class Step {
		SPAWN,
		ChangeNextBehavior,
	};
private:
	Step step_;
	Easing spawnEasing_;

public:
	//コンストラクタ
	EnemySpawn(BaseEnemy* boss);
	~EnemySpawn();


	void Update()override;
	void Debug()override;

};
