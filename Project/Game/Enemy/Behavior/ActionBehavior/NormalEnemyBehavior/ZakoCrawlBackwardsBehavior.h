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
    /// @param skipStumble true ならStumbleBackwardsをスキップして直接Crawlフェーズへ
    ZakoCrawlBackwardsBehavior(NormalEnemy* enemy, bool skipStumble = false);
    ~ZakoCrawlBackwardsBehavior() override;

    void Update() override;
    void Debug() override;

private:
    NormalEnemy* pNormalEnemy_;
    Phase phase_ = Phase::STUMBLE;
};
