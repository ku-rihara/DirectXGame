#include "ComboUIController.h"
#include "Frame/Frame.h"
// behavior
#include "Behavior/ComboCountUp.h"
#include "Behavior/ComboWait.h"
#include <cstdint>
#include <imgui.h>

void ComboUIController::Init() {

    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // Ui生成
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i] = std::make_unique<ComboUI>();
        comboSprites_[i]->Init(static_cast<ComboUI::ComboDigit>(i));
    }

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

void ComboUIController::AlphaAdaptForTime(float comboTime, float comboMaxTime) {

    alpha_ = std::clamp(comboTime / comboMaxTime, 0.0f, 1.0f);
}

///=========================================================
/// パラメータ調整
///==========================================================
void ComboUIController::AdjustParam() {
#ifdef _DEBUG

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Parameter"); // パラメータ

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        comboSprites_[i]->AdjustParam();
    }

#endif
}

///=========================================================
/// パラメータBind
///==========================================================
void ComboUIController::RegisterParams() {
}

void ComboUIController::ChangeBehavior(std::unique_ptr<BaseComboUIBehavior> behavior) {
    behavior_ = std::move(behavior);
}

void ComboUIController::ChangeCountUPAnimation() {
    ChangeBehavior(std::make_unique<ComboCountUP>(this));
}
