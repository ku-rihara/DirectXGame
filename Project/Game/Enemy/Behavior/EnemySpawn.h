#pragma once

#include"BaseEnemyMoveBehavior.h"
#include"EasingFunction.h"
#include"Easing.h"

class EnemySpawn : public BaseEnemyMoveBehavior {
public:
    // コンストラクタ
    EnemySpawn(BaseEnemy* boss);
    ~EnemySpawn();

    void Update() override;
    void Debug() override;

private:
	enum class Step {
		EFFECTEMIT,
		SPAWN,
		ChangeNextBehavior,
	};
private:
	Step step_;
	Easing<Vector3> spawnEasing_;
    Vector3 tempEnemyScale_;
	

};
