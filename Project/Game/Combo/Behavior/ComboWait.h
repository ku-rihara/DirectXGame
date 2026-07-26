#pragma once

#include "BaseComboUIBehavior.h"


/// <summary>
/// コンボUIの待機状態の振る舞い
/// </summary>
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
