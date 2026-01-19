#include "PlayerComboAttackData.h"
// input
#include "input/Input.h"
#include "input/InputData.h"
// imGui
#include <imgui.h>

void PlayerComboAttackData::Init(const std::string& attackName) {
    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();

    // グループ作成、パラメータ登録、同期
    groupName_ = attackName;
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // conditionをIntから適応
    attackParam_.triggerParam.condition = static_cast<TriggerCondition>(triggerConditionInt_);

    // タイムラインの初期化
    timeLine_.Init(this);
}

void PlayerComboAttackData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);

    // タイムラインを再初期化
    timeLine_.Init(this);
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
    globalParameter_->Regist(groupName_, "isMotionOnly", &attackParam_.isMotionOnly);

    // CollisionParam
    globalParameter_->Regist(groupName_, "collisionSize", &attackParam_.collisionParam.size);
    globalParameter_->Regist(groupName_, "collisionOffsetPos", &attackParam_.collisionParam.offsetPos);
    globalParameter_->Regist(groupName_, "collisionStartTime", &attackParam_.collisionParam.startTime); // 追加
    globalParameter_->Regist(groupName_, "adaptTime", &attackParam_.collisionParam.adaptTime);
    globalParameter_->Regist(groupName_, "loopWaitTime", &attackParam_.collisionParam.loopWaitTime);
    globalParameter_->Regist(groupName_, "loopNum", &attackParam_.collisionParam.loopNum);
    globalParameter_->Regist(groupName_, "isAlwaysFollowing", &attackParam_.collisionParam.isAlwaysFollowing);

    // MoveParam
    globalParameter_->Regist(groupName_, "moveValue", &attackParam_.moveParam.value);
    globalParameter_->Regist(groupName_, "moveEaseType", &attackParam_.moveParam.easeType);
    globalParameter_->Regist(groupName_, "moveStartTime", &attackParam_.moveParam.startTime); // 追加
    globalParameter_->Regist(groupName_, "moveEaseTime", &attackParam_.moveParam.easeTime);
    globalParameter_->Regist(groupName_, "isAbleInputMoving", &attackParam_.moveParam.isAbleInputMoving);
    globalParameter_->Regist(groupName_, "isPositionYSelect", &attackParam_.moveParam.isPositionYSelect);
    globalParameter_->Regist(groupName_, "finishTimeOffset", &attackParam_.moveParam.finishTimeOffset);

    // TriggerParam
    globalParameter_->Regist(groupName_, "gamePadBottom", &attackParam_.triggerParam.gamePadBottom);
    globalParameter_->Regist(groupName_, "keyBordBottom", &attackParam_.triggerParam.keyBordBottom);
    globalParameter_->Regist(groupName_, "Condition", &triggerConditionInt_);
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
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (groupName_ + " Editing").c_str());

    ImGui::SeparatorText("攻撃編集");

    ShowParameters();
    // 　タイムライン描画
    timeLine_.Draw();

    ImGui::PopID();

#endif // _DEBUG
}

void PlayerComboAttackData::ShowParameters() {
    // ===== 常に表示するパラメータ =====

    ImGui::SeparatorText("parameters");

    ImGui::Checkbox("モーションのみ有効", &attackParam_.isMotionOnly);

    ImGui::SeparatorText("Trigger Conditions");

    // TriggerParam（常に表示）
    ImGui::Checkbox("最初の攻撃", &attackParam_.triggerParam.isFirstAttack);
    ImGui::Checkbox("前の攻撃がヒットした時が発動条件", &attackParam_.triggerParam.requireHit);
    ImGuiKeyboardKeySelector("キーボード:ボタン", attackParam_.triggerParam.keyBordBottom);
    ImGuiGamepadButtonSelector("パッド:ボタン", attackParam_.triggerParam.gamePadBottom);

    // 発動条件
    const char* conditionItems[] = {"地面", "空中", "両方"};
    triggerConditionInt_         = static_cast<int>(attackParam_.triggerParam.condition);
    if (ImGui::Combo("発動できる状況", &triggerConditionInt_,
            conditionItems, IM_ARRAYSIZE(conditionItems))) {
        attackParam_.triggerParam.condition = static_cast<TriggerCondition>(triggerConditionInt_);
    }

    ImGui::SeparatorText("Flow Control");

    ImGui::Checkbox("自動で次の攻撃に進む", &attackParam_.timingParam.isAutoAdvance);
    ImGui::Checkbox("攻撃終了時に落ちる", &attackParam_.fallParam.enableFall);

    ImGui::SeparatorText("Next Attack");
    SelectNextAttack();

    ImGui::Separator();

    // セーブ・ロード
    ImGui::Separator();
    globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
    globalParameter_->ParamLoadForImGui(groupName_, folderPath_);
}

void PlayerComboAttackData::SelectNextAttack() {
    fileSelector_.SelectFile(
        "次の攻撃名",
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