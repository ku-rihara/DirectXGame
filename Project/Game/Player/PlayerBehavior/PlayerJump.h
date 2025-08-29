#pragma once

#include "BasePlayerBehavior.h"

class PlayerJump : public BasePlayerBehavior {
private:
    float speed_;
    enum class Step {
        START,
        JUMP,

    };
    /// ===================================================
    /// private varians
    /// ===================================================
    Step step_ = Step::START;

public:
    // コンストラクタ
    PlayerJump(Player* player);
    ~PlayerJump();

    void Update() override;

    void Debug() override;
};
