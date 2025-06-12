
#include "ComboCountUP.h"
#include "Combo/ComboUIController.h"
#include "ComboWait.h"
/// frame
#include "Frame/Frame.h"
/// inupt
#include "JoyState/JoyState.h"
/// imgui
#include <imgui.h>

// 初期化
ComboCountUP::ComboCountUP(ComboUIController* uiController)
    : BaseComboUIBehavior("ComboCountUP", uiController) {

    pUIController_->ScalingInit();

    // パラメータ初期化
    scalingEasing_.time      = 0.0f;
    scalingEasing_.maxTime   = pUIController_->GetScalingParameter().scalingEasing.maxTime;
    scalingEasing_.amplitude = pUIController_->GetScalingParameter().scalingEasing.amplitude;
    scalingEasing_.period    = pUIController_->GetScalingParameter().scalingEasing.period;
    amplitudeScale_          = pUIController_->GetScalingParameter().amplitudeScale;

    // 　step
    animationStep_ = AnimationStep::INIT;
}

ComboCountUP ::~ComboCountUP() {
}

// 更新
void ComboCountUP::Update() {

    switch (animationStep_) {
        ///-----------------------------------------------------------------------
        /// 初期化
        ///-----------------------------------------------------------------------
    case ComboCountUP::AnimationStep::INIT:
        animationStep_ = AnimationStep::UPDATE;
        break;
        ///-----------------------------------------------------------------------
        /// 更新
        ///-----------------------------------------------------------------------
    case ComboCountUP::AnimationStep::UPDATE:
        ScalingEasing();
        break;
        ///-----------------------------------------------------------------------
        /// 終了
        ///-----------------------------------------------------------------------
    case ComboCountUP::AnimationStep::END:

        pUIController_->ChangeBehavior(std::make_unique<ComboWait>(pUIController_));
        break;
    default:
        break;
    }
}

void ComboCountUP::Debug() {
    ImGui::Text("ComboCountUP");
}

void ComboCountUP::ScalingEasing() {
    scalingEasing_.time += Frame::DeltaTime();

    // base Scaleの更新
    baseScale_ = EaseAmplitudeScale(
        amplitudeScale_, scalingEasing_.time,
        scalingEasing_.maxTime, scalingEasing_.amplitude,
        scalingEasing_.period);

    // base Scaleの適応
    pUIController_->SetBaseScale(baseScale_);

    if (scalingEasing_.time < scalingEasing_.maxTime) {
        return;
    }

    scalingEasing_.time = scalingEasing_.maxTime;
    baseScale_          = amplitudeScale_;
    // base Scaleの適応
    pUIController_->SetBaseScale(baseScale_);

    animationStep_ = AnimationStep::END;
}
