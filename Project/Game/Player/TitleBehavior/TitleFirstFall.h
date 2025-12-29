#pragma once

#include "BaseTitleBehavior.h"
#include "Easing/Easing.h"

/// <summary>
/// タイトル最初の落下振る舞いクラス
/// </summary>
class TitleFirstFall : public BaseTitleBehavior {
public:
    // コンストラクタ
    TitleFirstFall(Player* player);
    ~TitleFirstFall();

    void Update() override; //< 更新
    void Debug() override; //< デバッグ
    void EasingInit(); //< イージングの初期化

private:
    enum class STEP {
        FALL,
        LANDING,
        WAIT,
        RETURNROOT,
    };

private:
    /// ===================================================
    /// private variable
    /// ===================================================
    STEP step_;

    /// fall
    float fallInitPosLHand_;
    float fallInitPosRHand_;
    float fallEaseT_;
    float fallRotateY_;

    /// landing
    float boundSpeed_;
    Vector3 initRotate_;
    float boundFallSpeedLimit_;
    float landRotateX_;
    float rotateXSpeed_;
    float rotateYSpeed_;
    float gravity_;

    KetaEngine::Easing<Vector3> landScaleEasing_;
    Vector3 tempScale_;
    KetaEngine::Easing<float> fallEase_;
    float tempPosY_;

    // float fallSpeed_;
    float waitTime_;
};