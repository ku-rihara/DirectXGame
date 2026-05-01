#pragma once

#include "../BaseEnemyBehavior.h"

class NormalEnemy;

/// <summary>
/// ボスが倒されたときにザコ敵が後ずさりする
/// StumbleBackwards → CrawlBackwards(ループ) の順で再生
/// </summary>
class ZakoCrawlBackwardsBehavior : public BaseEnemyBehavior {
    enum class Phase { STUMBLE, CRAWL };

public:
    ZakoCrawlBackwardsBehavior(NormalEnemy* enemy);
    ~ZakoCrawlBackwardsBehavior() override;

    void Update() override;
    void Debug() override;

private:
    NormalEnemy* pNormalEnemy_;
    float fleeTimer_ = 0.0f;
    Phase phase_     = Phase::STUMBLE;
};
