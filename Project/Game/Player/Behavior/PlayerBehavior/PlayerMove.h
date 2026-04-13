#pragma once

#include "BasePlayerBehavior.h"
#include "Easing/Easing.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"

/// <summary>
/// プレイヤー移動振る舞いクラス
/// </summary>
class PlayerMove : public BasePlayerBehavior {
public:
    // コンストラクタ
    PlayerMove(Player* player);
    ~PlayerMove();

    // 更新、デバッグ
    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

    void MoveAnimation(); //< 移動アニメーション
    void WaitAnimation(); //< 待機アニメーション
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
    KetaEngine::Easing<float> waitEase_;
    float tempWaitScaleY_ = 1.0f;
    float moveEaseTime_;

    //イージングアニメーション
    KetaEngine::ObjEaseAnimationPlayer easeAnimationPlayer_;

    float tempPosY_ = 0.0f;
    Vector3 tempScale_;
};
