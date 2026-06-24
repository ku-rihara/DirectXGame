#pragma once

#include "BaseBehavior/BaseBehavior.h"

class Player;
class PlayerParameter;

/// <summary>
/// プレイヤー振る舞いの基底クラス
/// </summary>
class BasePlayerBehavior : public BaseBehavior<Player> {
public:
    enum class Type { Base, Move, Dash, Jump, Death, Spawn };

    BasePlayerBehavior(const std::string& name, Player* player);

    virtual ~BasePlayerBehavior() {}

    virtual void Update(float timeSpeed = 0.0f) override = 0;
    virtual void Debug() override                        = 0;
    virtual Type GetBehaviorType() const { return Type::Base; }
protected:
    PlayerParameter* pPlayerParameter_ = nullptr;
};