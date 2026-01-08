#pragma once

#include "BasePlayerBehavior.h"
#include <functional>

/// <summary>
/// プレイヤージャンプ振る舞いクラス
/// </summary>
class PlayerJump : public BasePlayerBehavior {
private:
    // 各状態の処理関数
    void StartState();
    void JumpState();
    void FallOnlyState();

    // 状態遷移
    void ChangeState(std::function<void()> newState);

private:
    float speed_;
    bool skipJump_ = false;

    // 状態関数
    std::function<void()> currentState_;

public:
    // コンストラクタ
    PlayerJump(Player* player, const bool& skipJump = false);
    ~PlayerJump();

    void Update([[maybe_unused]] float timeSpeed = 0.0f) override; //< 更新
    void Debug() override; //< デバッグ
};