#pragma once

#include "BasePlayerBehavior.h"
#include "Easing.h"

class PlayerSpawn : public BasePlayerBehavior {
public:
    // コンストラクタ
    PlayerSpawn(Player* player);
    ~PlayerSpawn();

    void Update() override;
    void Debug() override;
private:
    enum class AnimationStep {
        INIT,
        SPAWN,
        END,     
    };

private:
    /// ===================================================
    /// private variable
    /// ===================================================

    AnimationStep animationStep_;
    float speed_;

    float animationCollTime_;
    Easing<float> waitEase_;
    float tempWaitScaleY_;

    float moveEaseTime_;
};
