#pragma once

#include "BasePlayerBehavior.h"
#include <functional>

/// <summary>
/// プレイヤー死亡
/// </summary>
class PlayerDeath : public BasePlayerBehavior {
public:
    PlayerDeath(Player* player);
    ~PlayerDeath();

    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;
    Type GetBehaviorType() const override { return Type::Death; }

private:
    void StartDeathRendition(float timeSpeed);
    void WaitCameraEnd(float timeSpeed);
    void WaitPreDeath(float timeSpeed);

    void ChangeState(const std::function<void(float)>& newState);

private:
    std::function<void(float)> currentState_;

    bool isPreDeathAnimDone_       = false;
    bool isDeathActionsTriggered_  = false;
};
