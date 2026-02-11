#pragma once

#include "BaseTitleBehavior.h"
#include "Easing/Easing.h"

/// <summary>
/// タイトル待機振る舞いクラス
/// </summary>
class TitleWait : public BaseTitleBehavior {

    enum class STEP {
        PRESS,
        RESTORATIVE,
    };

private:
    /// ===================================================
    /// private variable
    /// ===================================================
    STEP step_;
    KetaEngine::Easing<float> pressEase_;
    float tempScaleY_;
    float pressScale_;
    float waitTime_;

public:
    // コンストラクタ
    TitleWait(Player* player);
    ~TitleWait();

    void Update() override; //< 更新
    void Debug() override;  //< デバッグ
};