
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

    // easing 初期化
    scalingEasing_.Init("ComboUIScaling");
    scalingEasing_.SetAdaptValue(&baseScale_);
    scalingEasing_.Reset();

    scalingEasing_.SetOnFinishCallback([this]() {
        pUIController_->SetBaseScale(baseScale_);
        animationStep_ = AnimationStep::END;
    });

  
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
        scalingEasing_.Update(Frame::DeltaTime());
        pUIController_->SetBaseScale(baseScale_);
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

