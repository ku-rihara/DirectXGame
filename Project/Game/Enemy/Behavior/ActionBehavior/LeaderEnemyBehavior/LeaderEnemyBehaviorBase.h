#pragma once

#include "../BaseEnemyBehavior.h"

class LeaderEnemy;

/// <summary>
/// LeaderEnemy専用Behaviorの基底クラス
/// </summary>
class LeaderEnemyBehaviorBase : public BaseEnemyBehavior {
protected:
    /// <summary>
    /// LeaderEnemy*として取得
    /// </summary>
    LeaderEnemy* GetEnemy() const;

public:

    LeaderEnemyBehaviorBase(const std::string& name, LeaderEnemy* enemy);
    virtual ~LeaderEnemyBehaviorBase() = default;
};
