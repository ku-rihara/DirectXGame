#pragma once

#include "../BaseEnemyBehavior.h"

class StrongEnemy;

/// <summary>
/// StrongEnemy専用Behaviorの基底クラス
/// </summary>
class StrongEnemyBehaviorBase : public BaseEnemyBehavior {
protected:
    /// <summary>
    /// StrongEnemy*として取得
    /// </summary>
    StrongEnemy* GetEnemy() const;

public:

    StrongEnemyBehaviorBase(const std::string& name, StrongEnemy* enemy);
    virtual ~StrongEnemyBehaviorBase() = default;
};
