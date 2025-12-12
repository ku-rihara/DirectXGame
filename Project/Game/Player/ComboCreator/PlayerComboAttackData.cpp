#include "PlayerComboAttackData.h"
#include "Function/GetFile.h"
#include "input/Input.h"
#include "input/InputData.h"
#include <algorithm>
#include <imgui.h>

void PlayerComboAttackData::Init(const std::string& attackName) {
    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = attackName;
    globalParameter_->CreateGroup(groupName_);

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
    globalParameter_->Regist(groupName_, "power", &attackParam_.power);
    globalParameter_->Regist(groupName_, "KnockBackPower", &attackParam_.knockBackPower);
    globalParameter_->Regist(groupName_, "blowYPower", &attackParam_.blowYPower);

    // CollisionParam
    globalParameter_->Regist(groupName_, "collisionSize", &attackParam_.collisionParam.size);
    globalParameter_->Regist(groupName_, "collisionOffsetPos", &attackParam_.collisionParam.offsetPos);
    globalParameter_->Regist(groupName_, "adaptTime", &attackParam_.collisionParam.adaptTime);
    globalParameter_->Regist(groupName_, "loopWaitTime", &attackParam_.collisionParam.loopWaitTime);
    globalParameter_->Regist(groupName_, "loopNum", &attackParam_.collisionParam.loopNum);
    globalParameter_->Regist(groupName_, "isAlwaysFollowing", &attackParam_.collisionParam.isAlwaysFollowing);

    // MoveParam
    globalParameter_->Regist(groupName_, "moveValue", &attackParam_.moveParam.value);
    globalParameter_->Regist(groupName_, "moveEaseType", &attackParam_.moveParam.easeType);
    globalParameter_->Regist(groupName_, "moveEaseTime", &attackParam_.moveParam.easeTime);
    globalParameter_->Regist(groupName_, "isAbleInputMoving", &attackParam_.moveParam.isAbleInputMoving);
    globalParameter_->Regist(groupName_, "isPositionYSelect", &attackParam_.moveParam.isPositionYSelect);
    globalParameter_->Regist(groupName_, "finishTimeOffset", &attackParam_.moveParam.finishTimeOffset);

    // TriggerParam
    globalParameter_->Regist(groupName_, "gamePadBottom", &attackParam_.triggerParam.gamePadBottom);
    globalParameter_->Regist(groupName_, "keyBordBottom", &attackParam_.triggerParam.keyBordBottom);
    globalParameter_->Regist(groupName_, "Condition", &tempCondition_);
    globalParameter_->Regist(groupName_, "IsFirstAttack", &attackParam_.triggerParam.isFirstAttack);
    globalParameter_->Regist(groupName_, "RequireHit", &attackParam_.triggerParam.requireHit);
    globalParameter_->Regist(groupName_, "isAutoAdvance", &attackParam_.timingParam.isAutoAdvance);

    // TimingParam
    globalParameter_->Regist(groupName_, "isCancel", &attackParam_.timingParam.isCancel);
    globalParameter_->Regist(groupName_, "cancelFrame", &attackParam_.timingParam.cancelTime);
    globalParameter_->Regist(groupName_, "precedeInputFrame", &attackParam_.timingParam.precedeInputTime);
    globalParameter_->Regist(groupName_, "finishWaitTime", &attackParam_.timingParam.finishWaitTime);

    // FallParam
    globalParameter_->Regist(groupName_, "enableFall", &attackParam_.fallParam.enableFall);
  
    // nextAttack
    globalParameter_->Regist(groupName_, "NextAttackType", &attackParam_.nextAttackType);

    // 演出のパラメータバインド
    renditionData_.RegisterParams(globalParameter_, groupName_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerComboAttackData::AdjustParam() {
#ifdef _DEBUG

    ImGui::PushID(groupName_.c_str());

    // TriggerParam
    ImGui::Checkbox("isFirstAttack", &attackParam_.triggerParam.isFirstAttack);
    ImGui::Checkbox("Require Hit", &attackParam_.triggerParam.requireHit); 
    ImGuiKeyboardKeySelector("keyBoard:TriggerBottom", attackParam_.triggerParam.keyBordBottom);
    ImGuiGamepadButtonSelector("GamePad:TriggerBottom", attackParam_.triggerParam.gamePadBottom);

    // 発動条件
    const char* conditionItems[] = {"Ground", "Air", "Both"};
    tempCondition_               = static_cast<int>(attackParam_.triggerParam.condition);
    if (ImGui::Combo("Trigger Condition", &tempCondition_, conditionItems, IM_ARRAYSIZE(conditionItems))) {
        attackParam_.triggerParam.condition = static_cast<TriggerCondition>(tempCondition_);
    }

    // Simple Parameter
    ImGui::SeparatorText("simple Parameter");
    ImGui::DragFloat("Power", &attackParam_.power, 0.01f);
    ImGui::DragFloat("KnockBack Power", &attackParam_.knockBackPower, 0.01f);
    ImGui::DragFloat("Blow Y Power", &attackParam_.blowYPower, 0.01f);

    // Collision Parameter
    ImGui::SeparatorText("Collision Parameter");
    ImGui::DragFloat3("Collision Size", &attackParam_.collisionParam.size.x, 0.01f);
    ImGui::DragFloat3("Collision Offset Pos", &attackParam_.collisionParam.offsetPos.x, 0.01f);
    ImGui::DragFloat("Adapt Time", &attackParam_.collisionParam.adaptTime, 0.01f);
    ImGui::Checkbox("is Auto Advance to Next Attack", &attackParam_.timingParam.isAutoAdvance);
    ImGui::InputInt("Loop Num", &attackParam_.collisionParam.loopNum);
    if (attackParam_.collisionParam.loopNum > 0) {
        ImGui::DragFloat("Loop Wait Time", &attackParam_.collisionParam.loopWaitTime, 0.01f);
    }
    ImGui::Checkbox("is Always Following", &attackParam_.collisionParam.isAlwaysFollowing);

    // Move Parameter
    ImGui::SeparatorText("Move Parameter");
    ImGui::Checkbox("isAble InputMoving", &attackParam_.moveParam.isAbleInputMoving);
    ImGui::DragFloat("Move Ease Time", &attackParam_.moveParam.easeTime, 0.01f);
    ImGui::Checkbox("isPositionYSelect", &attackParam_.moveParam.isPositionYSelect);
    ImGui::DragFloat3("Move Value", &attackParam_.moveParam.value.x, 0.01f);
    ImGui::DragFloat("Finish Time Offset", &attackParam_.moveParam.finishTimeOffset, 0.01f);

    ImGuiEasingTypeSelector("Move Easing Type", attackParam_.moveParam.easeType);

    // Timing Parameters
    ImGui::SeparatorText("Timing Parameter");
    ImGui::DragFloat("Precede Input Time", &attackParam_.timingParam.precedeInputTime, 0.01f);
    ImGui::DragFloat("finish Wait Time", &attackParam_.timingParam.finishWaitTime, 0.01f);

    ImGui::Checkbox("is Add Cancel Time", &attackParam_.timingParam.isCancel);
    if (attackParam_.timingParam.isCancel) {
        ImGui::DragFloat("Cancel Time", &attackParam_.timingParam.cancelTime, 0.01f);
    }

    // Fall Parameters 
    ImGui::SeparatorText("Fall Parameter");
    ImGui::Checkbox("Enable Fall", &attackParam_.fallParam.enableFall);
   
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

bool PlayerComboAttackData::IsReserveNextAttack(float currentTime, const TriggerParam& nextAtkTrigger, bool hasHitEnemy) {

    // 先行入力受付
    if (currentTime < attackParam_.timingParam.precedeInputTime && !IsWaitFinish(currentTime)) {
        return false;
    }

    // ヒット条件チェック
    if (nextAtkTrigger.requireHit && !hasHitEnemy) {
        return false;
    }

    // キーボード入力チェック
    if (KetaEngine::Input::GetInstance()->TriggerKey(FromDIKCode(nextAtkTrigger.keyBordBottom))) {
        return true;
    }

    // ゲームパッド入力チェック
    if (KetaEngine::Input::IsTriggerPad(0, FromXInputButtonFlag(nextAtkTrigger.gamePadBottom))) {
        return true;
    }

    return false;
}

bool PlayerComboAttackData::IsCancelAttack(float currentTime, const TriggerParam& nextAtkTrigger, bool hasHitEnemy) {

    if (!attackParam_.timingParam.isCancel) {
        return false;
    }

    if (currentTime < attackParam_.timingParam.cancelTime) {
        return false;
    }

    // ヒット条件チェック
    if (nextAtkTrigger.requireHit && !hasHitEnemy) {
        return false;
    }

    // キーボード入力チェック
    if (KetaEngine::Input::GetInstance()->TriggerKey(FromDIKCode(nextAtkTrigger.keyBordBottom))) {
        return true;
    }

    // ゲームパッド入力チェック
    if (KetaEngine::Input::IsTriggerPad(0, FromXInputButtonFlag(nextAtkTrigger.gamePadBottom))) {
        return true;
    }

    return false;
}

bool PlayerComboAttackData::IsWaitFinish(float currentTime) {
    if (currentTime >= attackParam_.timingParam.finishWaitTime) {
        return true;
    } else {
        return false;
    }
}