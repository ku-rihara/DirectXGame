#pragma once

#include "BaseBehavior/BaseBehavior.h"

class Player;
class PlayerParameter;

/// <summary>
/// プレイヤー振る舞いの基底クラス
/// </summary>
class BasePlayerBehavior : public BaseBehavior<Player> {
public:
    BasePlayerBehavior(const std::string& name, Player* player);

    virtual ~BasePlayerBehavior() {}

    virtual void Update(float timeSpeed = 0.0f) override = 0; //< 更新
    virtual void Debug() override                        = 0; //< デバッグ
protected:
    PlayerParameter* pPlayerParameter_ = nullptr;
};