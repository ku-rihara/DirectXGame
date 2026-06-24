#pragma once

#include "BasePlayerBehavior.h"
#include "Editor/Easing/Easing.h"

/// <summary>
/// プレイヤー出現振る舞いクラス
/// </summary>
class PlayerSpawn : public BasePlayerBehavior {
public:
    // コンストラクタ
    PlayerSpawn(Player* player);
    ~PlayerSpawn();

    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;
    Type GetBehaviorType() const override { return Type::Spawn; }

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

    Step step_;
    float speed_;

    float animationCollTime_;
    KetaEngine::Easing<float> waitEase_;
    float tempWaitScaleY_;

    float moveEaseTime_;
};