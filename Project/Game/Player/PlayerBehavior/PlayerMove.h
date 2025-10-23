#pragma once

#include "BasePlayerBehavior.h"
#include "Easing/Easing.h"
#include "Easing/EasingSequence.h"

/// <summary>
/// プレイヤー移動振る舞いクラス
/// </summary>
class PlayerMove : public BasePlayerBehavior {
public:
    // コンストラクタ
    PlayerMove(Player* player);
    ~PlayerMove();

    void MoveAnimation(); //< 移動アニメーション
    void WaitAnimation(); //< 待機アニメーション
    void Update() override; //< 更新
    void Debug() override; //< デバッグ
    void JumpForJoyState(); //< ジャンプ待機状態

private:
    enum class AnimationStep {
        INIT,
        UPDATE,
        COOLING,
    };

private:
    /// ===================================================
    /// private variable
    /// ===================================================

    AnimationStep animationStep_;
    float speed_;

    float animationCollTime_;
    Easing<float> waitEase_;
    std::unique_ptr<EasingSequence> moveEase_;
    float tempWaitScaleY_;
    float moveEaseTime_;

    float tempPosY_ = 0.0f;
    Vector3 tempScale_;
};