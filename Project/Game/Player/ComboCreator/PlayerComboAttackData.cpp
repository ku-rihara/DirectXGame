#include "PlayerComboAttackData.h"
#include <imgui.h>

void PlayerComboAttackData::Init(const std::string& attackName) {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = attackName;
    globalParameter_->CreateGroup(groupName_, true);

    // 重複バインドを防ぐ
    globalParameter_->ClearBindingsForGroup(groupName_);
    BindParams();

    // パラメータ同期
    globalParameter_->SyncParamForGroup(groupName_);
}

void PlayerComboAttackData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void PlayerComboAttackData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

///==========================================================
/// バインド
///==========================================================
void PlayerComboAttackData::BindParams() {
    globalParameter_->Bind(groupName_, "offsetValue", &attackParam_.collisionOffsetValue);
    globalParameter_->Bind(groupName_, "ColliderSize", &attackParam_.collisionSize);
    globalParameter_->Bind(groupName_, "adaptTime", &attackParam_.adaptTime);
    globalParameter_->Bind(groupName_, "power", &attackParam_.power);
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerComboAttackData::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Attack Parameter");

        ImGui::DragFloat3("CollisionSize", &attackParam_.collisionSize.x, 0.01f);
        ImGui::DragFloat("offsetValue", &attackParam_.collisionOffsetValue, 0.01f);
        ImGui::DragFloat("adaptTime", &attackParam_.adaptTime, 0.01f);
        ImGui::DragFloat("power", &attackParam_.power, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
        globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

        ImGui::PopID();
    }
#endif // _DEBUG
}