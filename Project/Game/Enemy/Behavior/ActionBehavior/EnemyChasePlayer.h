#pragma once

#include "BaseEnemyBehavior.h"
#include "Easing/Easing.h"

class EnemyChasePlayer : public BaseEnemyBehavior {
private:
    float distance_;

    bool isChase_;
    bool isChaseStarted_; // 追跡開始フラグ（アニメーション制御用）

    KetaEngine::Easing<Vector3> scaleEasing_;
    Vector3 tempEnemyScale_;

public:
    // コンストラクタ
    EnemyChasePlayer(BaseEnemy* boss);
    ~EnemyChasePlayer();

    void Update() override;
    void Debug() override;
};
