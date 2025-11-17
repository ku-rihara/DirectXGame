#include "EnemyDamageReactionData.h"
#include "Function/GetFile.h"
#include "input/InputData.h"
#include <algorithm>
#include <imgui.h>

void EnemyDamageReactionData::Init(const std::string& attackName) {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = attackName;
    globalParameter_->CreateGroup(groupName_);

    // バインド
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void EnemyDamageReactionData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void EnemyDamageReactionData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

///==========================================================
/// バインド
///==========================================================
void EnemyDamageReactionData::RegisterParams() {
   
    // TimingParam
    globalParameter_->Regist(groupName_, "cancelFrame", &reactionParam_.timingParam.cancelFrame);
    globalParameter_->Regist(groupName_, "precedeInputFrame", &reactionParam_.timingParam.precedeInputFrame);

    // 演出のパラメータバインド
    renditionData_.BindParams(globalParameter_, groupName_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void EnemyDamageReactionData::AdjustParam() {
#ifdef _DEBUG

    ImGui::PushID(groupName_.c_str());

   
    // Other Parameters
    ImGui::SeparatorText("Timing Parameter");
    ImGui::DragFloat("Cancel Frame", &reactionParam_.timingParam.cancelFrame, 0.01f);
    ImGui::DragFloat("Precede Input Frame", &reactionParam_.timingParam.precedeInputFrame, 0.01f);

    // next Attack
    ImGui::SeparatorText("Next Attack");
    SelectNextAttack();

    renditionData_.AdjustParam();

    // セーブ・ロード
    ImGui::Separator();
    globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
    globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

    ImGui::PopID();

#endif // _DEBUG
}


void EnemyDamageReactionData::SelectNextAttack() {
    fileSelector_.SelectFile(
        "Next Attack Type",
        "Resources/GlobalParameter/EnemyDamageReaction",
        reactionParam_.nextAttackType,
        groupName_, 
        true);
}
