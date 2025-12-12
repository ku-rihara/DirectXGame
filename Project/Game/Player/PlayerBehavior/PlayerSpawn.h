#pragma once

#include "BasePlayerBehavior.h"
#include "Easing/Easing.h"

/// <summary>
/// プレイヤー出現振る舞いクラス
/// </summary>
class PlayerSpawn : public BasePlayerBehavior {
public:
    // コンストラクタ
    PlayerSpawn(Player* player);
    ~PlayerSpawn();

    void Update() override; //< 更新
    void Debug() override; //< デバッグ

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

    float startWaitTime_;
    KetaEngine::Easing<float> spawnEase_;
    float tempDessolve_;

    Step step_;
    float speed_;

    float animationCollTime_;
    KetaEngine::Easing<float> waitEase_;
    float tempWaitScaleY_;

    float moveEaseTime_;
};