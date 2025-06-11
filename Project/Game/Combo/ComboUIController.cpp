#include "ComboUIController.h"
#include "Frame/Frame.h"
// behavior
#include"Behavior/ComboCountUp.h"
#include <cstdint>
#include <imgui.h>

void ComboUIController::Init() {

    // Ui生成
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i] = std::make_unique<ComboUI>();
    }

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncGroupFromUI(groupName_);
}

void ComboUIController::Update(const int32_t&comboNum) {
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i]->CalculateNumber(comboNum);
    }
}
void ComboUIController::Draw() {
}

void ComboUIController::ScalingEasing() {
    scalingEasing_.time += Frame::DeltaTimeRate();

    baseScale_ = EaseAmplitudeScale(amplitudeScale_, scalingEasing_.time, scalingEasing_.maxTime, scalingEasing_.amplitude, scalingEasing_.period);

    if (scalingEasing_.time < scalingEasing_.maxTime) {
        return;
    }

    scalingEasing_.time = scalingEasing_.maxTime;
    baseScale_          = amplitudeScale_;
}

///=========================================================
/// パラメータ調整
///==========================================================
void ComboUIController::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Parameter"); //  パラメータ
        ImGui::DragFloat2("amplitudeScale_", &amplitudeScale_.x, 0.01f);

        ImGui::SeparatorText("Easing Parameter"); // Easing パラメータ
        ImGui::DragFloat("Scaling Ease max", &scalingEasing_.maxTime, 0.01f);
        ImGui::DragFloat("Scaling Ease amplitude", &scalingEasing_.amplitude, 0.01f);
        ImGui::DragFloat("Scaling Ease period", &scalingEasing_.period, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    /// 
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i]->AdjustParam();
    }

#endif // _DEBUG
}

///=========================================================
/// パラメータBind
///==========================================================
void ComboUIController::BindParams() {

    globalParameter_->Bind(groupName_, "Scaling Ease max", &scalingEasing_.maxTime);
    globalParameter_->Bind(groupName_, "Scaling Ease amplitude", &scalingEasing_.amplitude);
    globalParameter_->Bind(groupName_, "Scaling Ease period", &scalingEasing_.period);

    globalParameter_->Bind(groupName_, "amplitudeScale_", &amplitudeScale_);
}

void ComboUIController::ChangeBehavior(std::unique_ptr<BaseComboUIBehavior> behavior) {
    behavior_ = std::move(behavior);
}

void ComboUIController::ChangeCountUPAnimation() {
    ChangeBehavior(std::make_unique<ComboCountUP>(this));
 }