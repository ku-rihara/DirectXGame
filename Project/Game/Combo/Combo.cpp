#include "Combo.h"
#include "Easing.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <imgui.h>

void Combo::Init() {

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
}

void Combo::Update() {
}
void Combo::Draw() {
}

///=====================================================
///  ロード
///=====================================================
void Combo::ParamLoadForImGui() {

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
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Combo Parameter");
        ImGui::DragFloat("Combo Time Max", &comboTimeMax_, 0.01f); // UIで即編集

        ImGui::SeparatorText("Easing Parameter");
        ImGui::DragFloat("Scaling Ease max", &scalingEasing_.maxTime, 0.01f);
        ImGui::DragFloat("Scaling Ease amplitude", &scalingEasing_.amplitude, 0.01f);
        ImGui::DragFloat("Scaling Ease period", &scalingEasing_.period, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        ParamLoadForImGui();

        ImGui::PopID();
    }
#endif // _DEBUG
}


void Combo::BindParams() {
    globalParameter_->Bind(groupName_, "comboTimeMax_", &comboTimeMax_);

    // Easingパラメータもついでにバインド
    globalParameter_->Bind(groupName_, "Scaling Ease max", &scalingEasing_.maxTime);
    globalParameter_->Bind(groupName_, "Scaling Ease amplitude", &scalingEasing_.amplitude);
    globalParameter_->Bind(groupName_, "Scaling Ease period", &scalingEasing_.period);
}
