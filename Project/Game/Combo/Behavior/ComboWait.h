#pragma once

#include "BaseComboUIBehavior.h"
#include"EasingFunction.h"

class ComboWait : public BaseComboUIBehavior {

public:
    // コンストラクタ
    ComboWait(ComboUIController* comboUI);
    ~ComboWait();

    void Update() override;
    void Debug() override;

private:
    /// ===================================================
    /// private variants
    /// ===================================================

   
};
