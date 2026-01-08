#pragma once

#include "BasePlayerBehavior.h"
#include "Easing/Easing.h"
#include <functional>

/// <summary>
/// プレイヤー死亡
/// </summary>
class PlayerDeath : public BasePlayerBehavior {
public:
    // コンストラクタ
    PlayerDeath(Player* player);
    ~PlayerDeath();

    void Update([[maybe_unused]] float timeSpeed = 0.0f) override; //< 更新
    void Debug() override; //< デバッグ
private:
   
    void ChangeState(std::function<void()> newState);
private:
    std::function<void()> currentState_;

private:
    /// ===================================================
    /// private variable
    /// ===================================================
};