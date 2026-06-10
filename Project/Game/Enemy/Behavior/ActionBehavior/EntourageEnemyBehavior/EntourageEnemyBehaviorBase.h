#pragma once

#include "../BaseEnemyBehavior.h"

class EntourageEnemy;

/// <summary>
/// EntourageEnemy専用Behaviorの基底クラス
/// </summary>
class EntourageEnemyBehaviorBase : public BaseEnemyBehavior {
protected:
    /// <summary>
    /// EntourageEnemy*として取得
    /// </summary>
    EntourageEnemy* GetEnemy() const;

public:

    EntourageEnemyBehaviorBase(const std::string& name, EntourageEnemy* enemy);
    virtual ~EntourageEnemyBehaviorBase() = default;
};
