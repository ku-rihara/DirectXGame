#pragma once

#include "../BaseEnemyBehavior.h"

class NormalEnemy;

/// <summary>
/// NormalEnemy専用Behaviorの基底クラス
/// </summary>
class NormalEnemyBehaviorBase : public BaseEnemyBehavior {
protected:
    /// <summary>
    /// NormalEnemy*として取得
    /// </summary>
    NormalEnemy* GetEnemy() const;

public:

    NormalEnemyBehaviorBase(const std::string& name, NormalEnemy* enemy);
    virtual ~NormalEnemyBehaviorBase() = default;
};
