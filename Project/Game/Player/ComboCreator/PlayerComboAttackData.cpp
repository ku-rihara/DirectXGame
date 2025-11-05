#include "PlayerComboAttackData.h"
#include "Function/GetFile.h"
#include "input/InputData.h"
#include <algorithm>
#include <imgui.h>

void PlayerComboAttackData::Init(const std::string& attackName) {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = attackName;
    globalParameter_->CreateGroup(groupName_, true);

    // バインド
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // condition適応
    attackParam_.triggerParam.condition = static_cast<TriggerCondition>(tempCondition_);
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
void PlayerComboAttackData::RegisterParams() {
    // simpleParam
    globalParameter_->Bind(groupName_, "power", &attackParam_.power);
    globalParameter_->Bind(groupName_, "KnockBackPower", &attackParam_.knockBackPower);

    // CollisionParam
    globalParameter_->Bind(groupName_, "collisionSize", &attackParam_.collisionPara.size);
    globalParameter_->Bind(groupName_, "collisionOffsetPos", &attackParam_.collisionPara.offsetPos);
    globalParameter_->Bind(groupName_, "adaptTime", &attackParam_.collisionPara.adaptTime);

    // MoveParam
    globalParameter_->Bind(groupName_, "moveValue", &attackParam_.moveParam.value);
    globalParameter_->Bind(groupName_, "moveEaseType", &attackParam_.moveParam.easeType);
    globalParameter_->Bind(groupName_, "moveEaseTime", &attackParam_.moveParam.easeTime);

    // TriggerParam
    globalParameter_->Bind(groupName_, "gamePadBottom", &attackParam_.triggerParam.gamePadBottom);
    globalParameter_->Bind(groupName_, "keyBordBottom", &attackParam_.triggerParam.keyBordBottom);
    globalParameter_->Bind(groupName_, "Condition", &tempCondition_);
    globalParameter_->Bind(groupName_, "IsFirstAttack", &attackParam_.triggerParam.isFirstAttack);

    // TimingParam
    globalParameter_->Bind(groupName_, "cancelFrame", &attackParam_.timingParam.cancelFrame);
    globalParameter_->Bind(groupName_, "precedeInputFrame", &attackParam_.timingParam.precedeInputFrame);

    // nextAttack
    globalParameter_->Bind(groupName_, "NextAttackType", &attackParam_.nextAttackType);

    // 演出のパラメータバインド
    renditionData_.BindParams(globalParameter_, groupName_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerComboAttackData::AdjustParam() {
#ifdef _DEBUG

    ImGui::PushID(groupName_.c_str());

    // TriggerParam
    ImGui::Checkbox("isFirstAttack", &attackParam_.triggerParam.isFirstAttack);
    ImGuiKeyboardKeySelector("keyBoard:TriggerBottom", attackParam_.triggerParam.keyBordBottom);
    ImGuiGamepadButtonSelector("GamePad:TriggerBottom", attackParam_.triggerParam.gamePadBottom);
    const char* conditionItems[] = {"Ground", "Air", "Both"};
    tempCondition_               = static_cast<int>(attackParam_.triggerParam.condition);
    if (ImGui::Combo("Trigger Condition", &tempCondition_, conditionItems, IM_ARRAYSIZE(conditionItems))) {
        attackParam_.triggerParam.condition = static_cast<TriggerCondition>(tempCondition_);
    }

    // Simple Parameter
    ImGui::SeparatorText("simple Parameter");
    ImGui::DragFloat("Power", &attackParam_.power, 0.01f);
    ImGui::DragFloat("KnockBack Power", &attackParam_.knockBackPower, 0.01f);

    // Collision Parameter
    ImGui::SeparatorText("Collision Parameter");
    ImGui::DragFloat3("Collision Size", &attackParam_.collisionPara.size.x, 0.01f);
    ImGui::DragFloat3("Collision Offset Pos", &attackParam_.collisionPara.offsetPos.x, 0.01f);
    ImGui::DragFloat("Adapt Time", &attackParam_.collisionPara.adaptTime, 0.01f);

    // Move Parameter
    ImGui::SeparatorText("Move Parameter");
    ImGui::DragFloat("Move Ease Time", &attackParam_.moveParam.easeTime, 0.01f);
    ImGui::DragFloat3("Move Value", &attackParam_.moveParam.value.x, 0.01f);
    ImGuiEasingTypeSelector("Move Easing Type", attackParam_.moveParam.easeType);

    // Other Parameters
    ImGui::SeparatorText("Timing Parameter");
    ImGui::DragFloat("Cancel Frame", &attackParam_.timingParam.cancelFrame, 0.01f);
    ImGui::DragFloat("Precede Input Frame", &attackParam_.timingParam.precedeInputFrame, 0.01f);

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


void PlayerComboAttackData::SelectNextAttack() {
    fileSelector_.SelectFile(
        "Next Attack Type",
        "Resources/GlobalParameter/AttackCreator",
        attackParam_.nextAttackType,
        groupName_, 
        true);
}
