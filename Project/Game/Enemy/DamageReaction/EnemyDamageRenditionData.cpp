#include "EnemyDamageRenditionData.h"
#include <cassert>
#include <imgui.h>

void EnemyDamageRenditionData::Init(const std::string& groupName, int32_t renditionIndex) {
    globalParameter_       = GlobalParameter::GetInstance();
    currentRenditionIndex_ = renditionIndex;

    // グループ名を設定
    std::string newGroupName = groupName + "_Rendition" + std::to_string(currentRenditionIndex_);
    groupName_               = newGroupName;

    folderPath_ += groupName;

    if (!globalParameter_->HasRegisters(groupName_)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
    }
}

void EnemyDamageRenditionData::Reset() {
    // 必要に応じてリセット処理
}

void EnemyDamageRenditionData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void EnemyDamageRenditionData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void EnemyDamageRenditionData::RegisterParams() {
    globalParameter_->Regist(groupName_, "FileName", &objAnimationParams_.first.fileName);
    globalParameter_->Regist(groupName_, "StartTiming", &objAnimationParams_.first.startTiming);
 }

void EnemyDamageRenditionData::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("Rendition: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    // オブジェクトアニメーションパラメータのUI
    SelectObjAnimationFile("Animation File", objAnimationParams_);
    ImGui::DragFloat("Start Timing", &objAnimationParams_.first.startTiming, 0.01f);
    ImGui::PopID();
#endif
}

void EnemyDamageRenditionData::SelectObjAnimationFile(
    const char* label,
    std::pair<ObjAnimationParam, FileSelector>& param) {
    param.second.SelectFile(label, objAnimationFolderPath_, param.first.fileName, "", true);
}