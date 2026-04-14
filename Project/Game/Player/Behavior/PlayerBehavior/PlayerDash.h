#pragma once

#include "BasePlayerBehavior.h"
#include <functional>

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
    // ダッシュ開始
    void StartDash();
    /// 最初のダッシュ状態
    void UpdateStartDash();
    /// 通常ダッシュ状態
    void UpdateNormalDash();

private:
    bool forceDash_ = false;
    std::function<void()> currentState_;
};
