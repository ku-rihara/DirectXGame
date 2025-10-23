#pragma once

#include "BasePlayerBehavior.h"

/// <summary>
/// プレイヤージャンプ振る舞いクラス
/// </summary>
class PlayerJump : public BasePlayerBehavior {
private:
    float speed_;
    enum class Step {
        START,
        JUMP,

    };
    /// ===================================================
    /// private variable
    /// ===================================================
    Step step_ = Step::START;

public:
    // コンストラクタ
    PlayerJump(Player* player);
    ~PlayerJump();

    void Update() override; //< 更新
    void Debug() override; //< デバッグ
};