#include "Combo.h"
#include "Easing.h"
#include"base/TextureManager.h"
#include <imgui.h>
#include <cstdint>

void Combo::Init() {
    //int32_t textureId = TextureManager::GetInstance()->LoadTexture();

    //// Sprite create
    //sprite_.reset(Sprite::Create())

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
void Combo::AdjustParm() {
   
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Combo Parameter"); // combo パラメータ
        ImGui::DragFloat("Combo Time Max", &comboTimeMax_, 0.01f);

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

void Combo::BindParams() {
    globalParameter_->Bind(groupName_, "comboTimeMax_", &comboTimeMax_);

  
    globalParameter_->Bind(groupName_, "Scaling Ease max", &scalingEasing_.maxTime);
    globalParameter_->Bind(groupName_, "Scaling Ease amplitude", &scalingEasing_.amplitude);
    globalParameter_->Bind(groupName_, "Scaling Ease period", &scalingEasing_.period);
}


