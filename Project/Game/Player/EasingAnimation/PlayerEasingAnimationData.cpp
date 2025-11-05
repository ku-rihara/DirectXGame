#include "Function/GetFile.h"
#include "input/InputData.h"
#include "PlayerEasingAnimationData.h"
#include <algorithm>
#include <imgui.h>

void PlayerEasingAnimationData::Init(const std::string& attackName) {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = attackName;
    globalParameter_->CreateGroup(groupName_, true);

    // バインド
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void PlayerEasingAnimationData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void PlayerEasingAnimationData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

///==========================================================
/// バインド
///==========================================================
void PlayerEasingAnimationData::RegisterParams() {
   
    // MoveParam
    globalParameter_->Bind(groupName_, "moveValue", &attackParam_.moveParam.value);
    globalParameter_->Bind(groupName_, "moveEaseType", &attackParam_.moveParam.easeType);
    globalParameter_->Bind(groupName_, "moveEaseTime", &attackParam_.moveParam.easeTime);
  
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerEasingAnimationData::AdjustParam() {
#ifdef _DEBUG

    ImGui::PushID(groupName_.c_str());

    // Move Parameter
    ImGui::SeparatorText("Move Parameter");
    ImGui::DragFloat("Move Ease Time", &attackParam_.moveParam.easeTime, 0.01f);
    ImGui::DragFloat3("Move Value", &attackParam_.moveParam.value.x, 0.01f);
    ImGuiEasingTypeSelector("Move Easing Type", attackParam_.moveParam.easeType);


    renditionData_.AdjustParam();

    // セーブ・ロード
    ImGui::Separator();
    globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
    globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

    ImGui::PopID();

#endif // _DEBUG
}


