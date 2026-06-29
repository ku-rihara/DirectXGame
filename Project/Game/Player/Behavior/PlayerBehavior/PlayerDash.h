#pragma once

#include "BasePlayerBehavior.h"
#include "Editor/Easing/Easing.h"
#include "Vector3.h"
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

private:
    // ダッシュ開始
    void StartDash();
    /// 最初のダッシュ状態
    void UpdateStartDash();
    /// 通常ダッシュ状態
    void UpdateNormalDash();

private:
    bool forceDash_   = false;
    bool isStartDash_ = false;
    std::function<void()> currentState_;

    KetaEngine::Easing<Vector3> dashEasing_;
    Vector3 dashCurrentPos_;

public:
    Type GetBehaviorType() const override { return Type::Dash; }
    void SetForceDash(bool force) { forceDash_ = force; }
    bool IsStartDash() const { return isStartDash_; }
};
