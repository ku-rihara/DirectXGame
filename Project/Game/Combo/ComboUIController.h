#pragma once

#include "ComboUI.h"
#include "Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
// behavior
#include "Behavior/BaseComboUIBehavior.h"
// std
#include <array>
#include <cstdint>
#include <memory>

class ComboUIController {

public:
    ComboUIController()  = default;
    ~ComboUIController() = default;

    // init update draw
    void Init();
    void Update(const int32_t& comboNum);
    void Draw();

    // easing
    void ScalingEasing();
    // behavior
    void ChangeBehavior(std::unique_ptr<BaseComboUIBehavior> behavior);
    void ChangeCountUPAnimation();
    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParam();
    void BindParams();

private:
    GlobalParameter* globalParameter_; /// グローバルパラメータ
    const std::string groupName_ = "ComboUI"; /// グループ名

    ///* parameter *//
    Easing scalingEasing_;
    Vector2 amplitudeScale_;

    ///* Variants *//
    Vector2 baseScale_;
    std::array<std::unique_ptr<ComboUI>, 3> comboSprites_; /// コンボスプライト

    ///* behavior *//
    std::unique_ptr<BaseComboUIBehavior> behavior_;

public: // acceccer

   /* const Parameter& GetParameter() const { return parameter_; }

    void SetBasePosition(const Vector2& pos) {*/
 
};
