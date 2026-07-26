#pragma once

#include"../BaseEnemyBehavior.h"

#include"Editor/Easing/Easing.h"

/// <summary>
/// 敵のスポーン演出(エフェクト再生からスケール復帰まで)を行う振る舞い
/// </summary>
class EnemySpawn : public BaseEnemyBehavior {
public:
    // コンストラクタ
    EnemySpawn(BaseEnemy* boss);
    ~EnemySpawn();

    void Update() override;
    void Debug() override;
    bool IsSpawn() const override { return true; }

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
