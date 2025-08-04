#include "Combo.h"
#include "Frame/Frame.h"
#include <imgui.h>

void Combo::Init() {

    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    uiController_ = std::make_unique<ComboUIController>();
    uiController_->Init();
}

void Combo::Update() {
    // コンボカウント
    uiController_->Update(comboCount_);
    // コンボタイマーの更新
    ComboTimerDecrement();
    // コンボタイマーによるアルファの調整
    uiController_->AlphaAdaptForTime(comboTime_,comboTimeMax_);
}

void Combo::Draw() {
    uiController_->Draw();
}

///=========================================================
/// パラメータ調整
///==========================================================
void Combo::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Combo Parameter"); // combo パラメータ
        ImGui::DragFloat("Combo Time Max", &comboTimeMax_, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif // _DEBUG

    uiController_->AdjustParam();
}

void Combo::BindParams() {
    globalParameter_->Bind(groupName_, "comboTimeMax_", &comboTimeMax_);
}

void Combo::ComboCountUP() {
    uiController_->ChangeCountUPAnimation();
    comboCount_++;
    comboTime_ = comboTimeMax_;
}

void Combo::ComboTimerDecrement() {
    comboTime_ -= Frame::DeltaTimeRate();
    if (comboTime_ < 0.0f) {
        comboTime_ = 0.0f;
        comboCount_ = 0; 
    }
}

