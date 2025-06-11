#pragma once

#include "2d/Sprite.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Combo/ComboUIController.h"
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

    void ComboCountUP(); /// コンボ数を増やす

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParam();
    void BindParams();

private:

    // ui controller
    std::unique_ptr<ComboUIController> uiController_; /// UIコントローラ

    GlobalParameter* globalParameter_; /// グローバルパラメータ
    const std::string groupName_ = "Combo"; /// グループ名

    ///* parameter *//
    float comboTime_; /// コンボ時間
    int32_t comboCount_; /// コンボ数
    float comboTimeMax_; /// コンボ時間の最大値

public:// accessor

    //* Getter*//
    int32_t GetComboCount() const { return comboCount_; }

};
