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
private:
    struct ScalingParameter {
        Easing scalingEasing;
        Vector2 amplitudeScale;
    };

public:
    ComboUIController()  = default;
    ~ComboUIController() = default;

    // init update draw
    void Init();
    void Update(const int32_t& comboNum);
    void Draw();

    void AlphaAdaptForTime(const float& comboTime, const float& comboMaxTime);

    // easing
    void ScalingEasing();
    void ScalingInit();
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
    ScalingParameter parameter_;

    ///* Variants *//
    Vector2 baseScale_;
    float alpha_;
    std::array<std::unique_ptr<ComboUI>, 3> comboSprites_;

    ///* behavior *//
    std::unique_ptr<BaseComboUIBehavior> behavior_;

public: // acceccer
    
    ///* getter *//
    const ScalingParameter& GetScalingParameter() const { return parameter_; }

    ///* setter *//
    void SetBaseScale(const Vector2& pos) { baseScale_ = pos; }
 
};
