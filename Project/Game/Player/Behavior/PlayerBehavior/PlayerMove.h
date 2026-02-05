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

     void Update([[maybe_unused]] float timeSpeed = 0.0f) override; //< 更新
    void Debug() override;                                          //< デバッグ

    void MoveAnimation(); //< 移動アニメーション
    void WaitAnimation(); //< 待機アニメーション
    void JumpForJoyState(); //< ジャンプ待機状態

private:
    enum class AnimationStep {
        INIT,
        UPDATE,
        COOLING,
    };

public:
    /// <summary>
    /// ダッシュ中かどうかを取得
    /// </summary>
    bool IsDashing() const { return isDashing_; }

private:
    /// ===================================================
    /// private variable
    /// ===================================================

    bool isDashing_ = false; // ダッシュ中フラグ

    AnimationStep animationStep_;
    float speed_;

    float animationCollTime_;
    KetaEngine::Easing<float> waitEase_;
    std::unique_ptr<KetaEngine::EasingSequence> moveEase_;
    float tempWaitScaleY_;
    float moveEaseTime_;

    float tempPosY_ = 0.0f;
    Vector3 tempScale_;
};