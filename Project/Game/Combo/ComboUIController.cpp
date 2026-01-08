#include "ComboUIController.h"
#include "Frame/Frame.h"
// behavior
#include "Behavior/ComboCountUp.h"
#include "Behavior/ComboWait.h"
#include <cstdint>
#include <imgui.h>

void ComboUIController::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
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

void ComboUIController::Update(int32_t comboNum) {

    behavior_->Update();

    // 各桁のuvを更新
    for (int32_t i = 0; i < comboSprites_.size(); ++i) {
        // 基準座標 + 桁ごとのオフセットを適用
        Vector2 digitPosition = basePosition_ + Vector2(digitOffset_.x * i, digitOffset_.y * i);

        comboSprites_[i]->Update(baseScale_, alpha_);
        comboSprites_[i]->CalculateNumber(comboNum);
        comboSprites_[i]->SetPosition(digitPosition);
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

        // 基準座標の調整
        ImGui::DragFloat2("Base Position", &basePosition_.x, 0.1f);
        // 桁間オフセットの調整
        ImGui::DragFloat2("Digit Offset", &digitOffset_.x, 0.1f);

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
    globalParameter_->Regist(groupName_, "basePosition", &basePosition_);
    globalParameter_->Regist(groupName_, "digitOffset", &digitOffset_);
}

void ComboUIController::ChangeBehavior(std::unique_ptr<BaseComboUIBehavior> behavior) {
    behavior_ = std::move(behavior);
}

void ComboUIController::ChangeCountUPAnimation() {
    ChangeBehavior(std::make_unique<ComboCountUP>(this));
}