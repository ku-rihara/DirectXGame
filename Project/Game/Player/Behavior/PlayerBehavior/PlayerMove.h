#pragma once

#include "BasePlayerBehavior.h"

/// <summary>
/// プレイヤー移動振る舞いクラス
/// </summary>
class PlayerMove : public BasePlayerBehavior {
public:
    PlayerMove(Player* player);
    ~PlayerMove();

    // 更新、デバッグ
    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

private:
    void CheckAndSwitchAnimation();
    void JumpForJoyState();

    bool wasMoving_ = false;
};
