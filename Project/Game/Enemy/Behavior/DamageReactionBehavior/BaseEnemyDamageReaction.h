#pragma once

#include "Vector3.h"
#include <string>

class BaseEnemy;
class BaseEnemyDamageReaction {
public:
    BaseEnemyDamageReaction(const std::string& name, BaseEnemy* boss);
    virtual ~BaseEnemyDamageReaction() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Debug()                 = 0;

protected:
    // 振る舞い名
    std::string name_;
    // ボス
    BaseEnemy* pBaseEnemy_ = nullptr;

    Vector3 direction_;
    float objectiveAngle_;
};