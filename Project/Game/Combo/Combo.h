#pragma once

#include "Combo/ComboUIController.h"
#include "utility/ParameterEditor/GlobalParameter.h"
// std
#include <cstdint>
#include <memory>

class Combo {
public:
    Combo()  = default;
    ~Combo() = default;

    // init update draw
    void Init();
    void Update();
    void Draw();

    void ComboCountUP();
    void ComboTimerDecrement();

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParam();
    void BindParams();

private:

    // UIコントローラ
    std::unique_ptr<ComboUIController> uiController_; 

    GlobalParameter* globalParameter_; // グローバルパラメータ
    const std::string groupName_ = "Combo"; // グループ名

    // parameter
    float comboTime_;    // コンボ時間
    int32_t comboCount_; // コンボ数
    float comboTimeMax_; // コンボ時間の最大値

public: // accessor

    // Getter
    int32_t GetComboCount() const { return comboCount_; }
};
