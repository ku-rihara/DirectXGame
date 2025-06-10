#include "Combo.h"
#include "Easing.h"
#include"base/TextureManager.h"
#include <imgui.h>
#include <cstdint>

void Combo::Init() {

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncGroupFromUI(groupName_);
}

void Combo::Update() {
}
void Combo::Draw() {
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
}

void Combo::BindParams() {
    globalParameter_->Bind(groupName_, "comboTimeMax_", &comboTimeMax_);

}


