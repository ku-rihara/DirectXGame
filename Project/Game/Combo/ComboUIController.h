#pragma once

#include "2d/Sprite.h"
#include "Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
// std
#include <array>
#include <cstdint>
#include <memory>

class ComboUI;
class Combo;
class ComboUIController {

public:
    ComboUIController()  = default;
    ~ComboUIController() = default;

    // init update draw
    void Init();
    void Update();
    void Draw();

    // easing
    void ScalingEasing();

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParam();
    void BindParams();

private:
    Combo* pCombo_;

private:
    GlobalParameter* globalParameter_; /// グローバルパラメータ
    const std::string groupName_ = "ComboUI"; /// グループ名

    ///* parameter *//
    Easing scalingEasing_;
    Vector2 baseScale_;
    Vector2 amplitudeScale_;

    ///* Variants *//
    std::array<std::unique_ptr<ComboUI>, 3> comboSprites_; /// コンボスプライト

public:// acceccer

    ///* Setter *//
    void SetCombo(Combo* combo);
};
