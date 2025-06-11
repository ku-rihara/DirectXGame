
#include "ComboCountUP.h"
#include "Combo/ComboUIController.h"
/// frame
#include "Frame/Frame.h"
/// inupt
#include "JoyState/JoyState.h"
/// imgui
#include <imgui.h>

// 初期化
ComboCountUP::ComboCountUP(ComboUIController* uiController)
    : BaseComboUIBehavior("ComboCountUP", uiController) {
}

ComboCountUP ::~ComboCountUP() {
}

// 更新
void ComboCountUP::Update() {


}

void ComboCountUP::Debug() {
    ImGui::Text("ComboCountUP");
}


//void ComboCountUP::ScalingEasing() {
//    scalingEasing_.time += Frame::DeltaTimeRate();
//
//    baseScale_ = EaseAmplitudeScale(amplitudeScale_, scalingEasing_.time, scalingEasing_.maxTime, scalingEasing_.amplitude, scalingEasing_.period);
//
//    if (scalingEasing_.time < scalingEasing_.maxTime) {
//        return;
//    }
//
//    scalingEasing_.time = scalingEasing_.maxTime;
//    baseScale_          = amplitudeScale_;
//}