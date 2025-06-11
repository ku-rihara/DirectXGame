#pragma once

#include "BaseComboUIBehavior.h"
#include "Easing.h"

class ComboCountUP : public BaseComboUIBehavior {
private:

    enum class AnimationStep {
        INIT,
        UPDATE,
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
    /// private varians
    /// ===================================================

    AnimationStep animationStep_;

};
