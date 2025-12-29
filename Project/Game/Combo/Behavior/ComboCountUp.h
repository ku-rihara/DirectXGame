#pragma once

#include "BaseComboUIBehavior.h"
#include"Easing/EasingFunction.h"
#include"Easing/Easing.h"

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

 
private:
    /// ===================================================
    /// private variables
    /// ===================================================

    AnimationStep animationStep_;
    KetaEngine::Easing<Vector2> scalingEasing_;
    Vector2 baseScale_;
};
