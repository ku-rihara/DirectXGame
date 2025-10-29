#include "PlayerComboAttackData.h"
#include <imgui.h>

void PlayerComboAttackData::Init(const std::string& attackName) {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = attackName;
    globalParameter_->CreateGroup(groupName_, true);

    // バインド
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
    // CollisionParam
    globalParameter_->Bind(groupName_, "collisionSize", &attackParam_.collisionPara.collisionSize);
    globalParameter_->Bind(groupName_, "collisionOffsetValue", &attackParam_.collisionPara.collisionOffsetValue);
    globalParameter_->Bind(groupName_, "adaptTime", &attackParam_.collisionPara.adaptTime);

    // MoveParam
    globalParameter_->Bind(groupName_, "moveValue", &attackParam_.moveParam.value);
    globalParameter_->Bind(groupName_, "moveEaseType", &attackParam_.moveParam.easeType);

    // KnockBackParam
    globalParameter_->Bind(groupName_, "knockBackSpeed", &attackParam_.knockBackParam.speed);
    globalParameter_->Bind(groupName_, "knockBackDecreaseEaseType", &attackParam_.knockBackParam.decreaseEaseType);

    // Other
    globalParameter_->Bind(groupName_, "cancelFrame", &attackParam_.cancelFrame);
    globalParameter_->Bind(groupName_, "precedeInputFrame", &attackParam_.precedeInputFrame);
    globalParameter_->Bind(groupName_, "power", &attackParam_.power);
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerComboAttackData::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        // Collision Parameter
        ImGui::SeparatorText("Collision Parameter");
        ImGui::DragFloat3("Collision Size", &attackParam_.collisionPara.collisionSize.x, 0.01f);
        ImGui::DragFloat("Collision Offset Value", &attackParam_.collisionPara.collisionOffsetValue, 0.01f);
        ImGui::DragFloat("Adapt Time", &attackParam_.collisionPara.adaptTime, 0.01f);

        // Move Parameter
        ImGui::SeparatorText("Move Parameter");
        ImGui::DragFloat3("Move Value", &attackParam_.moveParam.value.x, 0.01f);
        EasingTypeSelector("Move Easing Type", attackParam_.moveParam.easeType);

        // KnockBack Parameter
        ImGui::SeparatorText("KnockBack Parameter");
        ImGui::DragFloat("KnockBack Speed", &attackParam_.knockBackParam.speed, 0.01f);
        EasingTypeSelector("KnockBack Decrease Easing Type", attackParam_.knockBackParam.decreaseEaseType);

        // Other Parameters
        ImGui::SeparatorText("Other Parameters");
        ImGui::DragFloat("Cancel Frame", &attackParam_.cancelFrame, 0.01f);
        ImGui::DragFloat("Precede Input Frame", &attackParam_.precedeInputFrame, 0.01f);
        ImGui::DragFloat("Power", &attackParam_.power, 0.01f);

        // セーブ・ロード
        ImGui::Separator();
        globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
        globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void PlayerComboAttackData::EasingTypeSelector(const char* label, int32_t& target) {
    int type = static_cast<int32_t>(target);
    if (ImGui::Combo(label, &type, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
        target = type;
    }
}