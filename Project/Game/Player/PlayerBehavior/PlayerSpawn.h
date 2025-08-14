#pragma once

#include "BasePlayerBehavior.h"
#include "Easing/Easing.h"

class PlayerSpawn : public BasePlayerBehavior {
public:
    // コンストラクタ
    PlayerSpawn(Player* player);
    ~PlayerSpawn();

    void Update() override;
    void Debug() override;
private:
    enum class Step {
        WAIT,
        SPAWN,
        END,     
    };

private:
    /// ===================================================
    /// private variable
    /// ===================================================
    /// 
    
    float startWaitTime_;
    Easing<float> spawnEase_;
    float tempDessolve_;

    Step step_;
    float speed_;

    float animationCollTime_;
    Easing<float> waitEase_;
    float tempWaitScaleY_;

    float moveEaseTime_;
};
