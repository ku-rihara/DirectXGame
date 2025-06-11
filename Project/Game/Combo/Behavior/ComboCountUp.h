#pragma once

#include "BaseComboUIBehavior.h"
#include "Easing.h"

class ComboCountUP : public BaseComboUIBehavior {
private:
    enum class AnimationStep {
        INIT,
        UPDATE,
        END,
    };

public:
    // コンストラクタ
    ComboCountUP(ComboUIController* comboUI);
    ~ComboCountUP();

    void Update() override;
    void Debug() override;

    void ScalingEasing();

private:
    /// ===================================================
    /// private variables
    /// ===================================================

    AnimationStep animationStep_;

    Easing scalingEasing_;
    Vector2 amplitudeScale_;
    Vector2 baseScale_;
};
