#include "Combo.h"
#include "Easing.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <imgui.h>

void Combo::Init() {

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    AddParmGroup();
    ApplyGlobalParameter();
}

void Combo::Update() {
}
void Combo::Draw() {
}

///=====================================================
///  ロード
///=====================================================
void Combo::ParmLoadForImGui() {

    // ロードボタン
    if (ImGui::Button(std::format("Load {}", groupName_).c_str())) {

        globalParameter_->LoadFile(groupName_);
        // セーブ完了メッセージ
        ImGui::Text("Load Successful: %s", groupName_.c_str());
        ApplyGlobalParameter();
    }
}

///=====================================================
///  パラメータをグループに追加
///=====================================================
void Combo::AddParmGroup() {
    globalParameter_->AddItem(groupName_, "comboTimeMax_", comboTimeMax_);
    globalParameter_->AddItem(groupName_, "comboTimeMax_", comboTimeMax_);
}
///=====================================================
/// パラメータ値セット
///=====================================================
void Combo::SetValues() {

    globalParameter_->SetValue(groupName_, "comboTimeMax_", comboTimeMax_);
}

///=====================================================
///  ImGuiからパラメータを得る
///=====================================================
void Combo::ApplyGlobalParameter() {
    // paramCombies_ から値を取得
    comboTimeMax_ = globalParameter_->GetValue<float>(groupName_, "comboTimeMax_");
}

///=========================================================
/// パラメータ調整
///==========================================================
void Combo::AdjustParm() {
    SetValues();
#ifdef _DEBUG

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Combo Parameter"); // combo パラメータ
        ImGui::DragFloat("Combo Time Max", &comboTimeMax_, 0.01f);

        ImGui::SeparatorText("Easing Parameter"); // Easing パラメータ
        ImGui::DragFloat("Scaling Ease max", &scalingEasing_.maxTime, 0.01f);
        ImGui::DragFloat("Scaling Ease amplitude", &scalingEasing_.amplitude, 0.01f);
        ImGui::DragFloat("Scaling Ease period", &scalingEasing_.period, 0.01f);

        /// セーブとロード
        globalParameter_->ParmSaveForImGui(groupName_);
        ParmLoadForImGui();
        ImGui::PopID();
    }

#endif // _DEBUG
}