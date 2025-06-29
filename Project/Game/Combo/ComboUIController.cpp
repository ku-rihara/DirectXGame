#include "ComboUIController.h"
#include "Frame/Frame.h"
// behavior
#include "Behavior/ComboCountUp.h"
#include "Behavior/ComboWait.h"
#include <cstdint>
#include <imgui.h>

void ComboUIController::Init() {

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // Ui生成
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i] = std::make_unique<ComboUI>();
        comboSprites_[i]->Init(static_cast<ComboUI::ComboDigit>(i));
    }

    // scale init
    ScalingInit();
    ChangeBehavior(std::make_unique<ComboWait>(this));
}

void ComboUIController::Update(const int32_t& comboNum) {

    behavior_->Update();

    // 各桁のuvを更新
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i]->Update(baseScale_, alpha_);
        comboSprites_[i]->CalculateNumber(comboNum);
    }
}

void ComboUIController::Draw() {
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i]->Draw();
    }
}

void ComboUIController::ScalingEasing() {
   /* parameter_.scalingEasing.time += Frame::DeltaTimeRate();

    baseScale_ = EaseAmplitudeScale(parameter_.amplitudeScale, parameter_.scalingEasing.time, parameter_.scalingEasing.maxTime, parameter_.scalingEasing.amplitude, parameter_.scalingEasing.period);

    if (parameter_.scalingEasing.time < parameter_.scalingEasing.maxTime) {
        return;
    }

    parameter_.scalingEasing.time = parameter_.scalingEasing.maxTime;
    baseScale_                    = parameter_.amplitudeScale;
    ChangeBehavior(std::make_unique<ComboWait>(this));*/
}

void ComboUIController::ScalingInit() {
  /*  baseScale_                    = parameter_.amplitudeScale;
    parameter_.scalingEasing.time = 0.0f;*/
}

void ComboUIController::AlphaAdaptForTime(const float& comboTime, const float& comboMaxTime) {

    alpha_ = std::clamp(comboTime / comboMaxTime, 0.0f, 1.0f);
}

///=========================================================
/// パラメータ調整
///==========================================================
void ComboUIController::AdjustParam() {

#ifdef _DEBUG

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Parameter"); //  パラメータ
       

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG

    ///
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i]->AdjustParam();
    }
}

///=========================================================
/// パラメータBind
///==========================================================
void ComboUIController::BindParams() {

   
}

void ComboUIController::ChangeBehavior(std::unique_ptr<BaseComboUIBehavior> behavior) {
    behavior_ = std::move(behavior);
}

void ComboUIController::ChangeCountUPAnimation() {
    ChangeBehavior(std::make_unique<ComboCountUP>(this));
}
