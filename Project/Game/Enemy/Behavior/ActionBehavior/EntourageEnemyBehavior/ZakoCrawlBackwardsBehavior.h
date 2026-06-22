#pragma once

#include "../BaseEnemyBehavior.h"

class EntourageEnemy;

/// <summary>
/// ボスが倒されたときにザコ敵が後ずさりする
/// </summary>
class ZakoCrawlBackwardsBehavior : public BaseEnemyBehavior {
    enum class Phase { 
        STUMBLE, 
        CRAWL 
    };

public:
    ZakoCrawlBackwardsBehavior(EntourageEnemy* enemy, bool skipStumble = false);
    ~ZakoCrawlBackwardsBehavior() override;

    void Update() override;
    void Debug() override;

private:
    EntourageEnemy* pEntourageEnemy_;
    Phase phase_ = Phase::STUMBLE;
};
