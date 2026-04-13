#pragma once

#include "BasePlayerBehavior.h"

/// <summary>
/// プレイヤーダッシュ振る舞いクラス
/// </summary>
class PlayerDash : public BasePlayerBehavior {
public:
    PlayerDash(Player* player, bool forceDash = false);
    ~PlayerDash() = default;

    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

    void JumpForJoyState();

    void SetForceDash(bool force) { forceDash_ = force; }

private:
    bool forceDash_ = false;
};
