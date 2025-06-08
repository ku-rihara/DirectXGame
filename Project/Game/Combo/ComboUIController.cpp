#include "ComboUIController.h"
#include "Easing.h"
#include"base/TextureManager.h"
#include <imgui.h>
#include <cstdint>

void ComboUIController::Init() {

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncGroupFromUI(groupName_);
}

void ComboUIController::Update() {
}
void ComboUIController::Draw() {
}



///=========================================================
/// パラメータ調整
///==========================================================
void ComboUIController::AdjustParm() {
   
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Easing Parameter"); // Easing パラメータ
        ImGui::DragFloat("Scaling Ease max", &scalingEasing_.maxTime, 0.01f);
        ImGui::DragFloat("Scaling Ease amplitude", &scalingEasing_.amplitude, 0.01f);
        ImGui::DragFloat("Scaling Ease period", &scalingEasing_.period, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
       
        ImGui::PopID();
    }
#endif // _DEBUG
}

void ComboUIController::BindParams() {
   
  
    globalParameter_->Bind(groupName_, "Scaling Ease max", &scalingEasing_.maxTime);
    globalParameter_->Bind(groupName_, "Scaling Ease amplitude", &scalingEasing_.amplitude);
    globalParameter_->Bind(groupName_, "Scaling Ease period", &scalingEasing_.period);
}


