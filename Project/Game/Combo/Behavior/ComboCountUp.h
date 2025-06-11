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

private:

    /// ===================================================
    /// private varians
    /// ===================================================

    AnimationStep animationStep_;

};
