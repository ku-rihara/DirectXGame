#pragma once

#include"../BaseEnemyBehavior.h"

#include"Easing/Easing.h"

class EnemySpawn : public BaseEnemyBehavior {
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
    KetaEngine::Easing<Vector3> spawnEasing_;
    Vector3 tempEnemyScale_;
	

};
