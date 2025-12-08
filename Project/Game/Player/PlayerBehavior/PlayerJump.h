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
        FALL_ONLY, 
    };
    /// ===================================================
    /// private variable
    /// ===================================================
    Step step_     = Step::START;
    bool skipJump_ = false;

public:
    // コンストラクタ
    PlayerJump(Player* player,const bool& skipJump = false); 
    ~PlayerJump();

    void Update() override; //< 更新
    void Debug() override; //< デバッグ
};