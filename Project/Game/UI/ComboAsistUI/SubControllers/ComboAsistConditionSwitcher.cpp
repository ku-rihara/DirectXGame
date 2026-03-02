#include "ComboAsistConditionSwitcher.h"
#include "input/InputData.h"
#include "Input/Input.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
#include <algorithm>

void ComboAsistConditionSwitcher::Init(
    PlayerComboAttackController* attackController,
    ComboUIBuilder* uiBuilder,
    std::map<PlayerComboAttackData::TriggerCondition, ConditionUIData>* conditionDataMap,
    std::vector<PlayerComboAttackData::TriggerCondition>* availableConditions,
    PlayerComboAttackData::TriggerCondition* currentCondition) {

    pAttackController_   = attackController;
    pUiBuilder_          = uiBuilder;
    pConditionDataMap_   = conditionDataMap;
    pAvailableConditions_ = availableConditions;
    pCurrentCondition_   = currentCondition;
}

///==========================================================
/// 手動条件切替（十字キー右）
///==========================================================
bool ComboAsistConditionSwitcher::CheckConditionSwitchInput(bool isPanelOpen) {
    if (!isPanelOpen) {
        return false;
    }
    if (!KetaEngine::Input::IsTriggerPad(0, GamepadButton::DPadRight)) {
        return false;
    }
    if (pAvailableConditions_->size() <= 1) {
        return false;
    }

    auto found = std::find(pAvailableConditions_->begin(), pAvailableConditions_->end(), *pCurrentCondition_);
    size_t idx = 0;
    if (found != pAvailableConditions_->end()) {
        idx = static_cast<size_t>(found - pAvailableConditions_->begin());
    }

    isAutoSwitchedCondition_ = false;
    // 切替はControllerが行うため、次のConditionを返す形にする
    // ここではSwitchConditionをController経由で呼ぶため、戻り値で知らせる
    return true; // Controller側でSwitchConditionを呼ぶ
}

///==========================================================
/// 自動条件切替（空中攻撃検知）
///==========================================================
bool ComboAsistConditionSwitcher::CheckAutoConditionSwitch(
    bool isVisible,
    Player* pPlayer,
    const std::string& prevBehaviorName) {

    using TC = PlayerComboAttackData::TriggerCondition;
    if (!isVisible || !pPlayer || !pAttackController_ || *pCurrentCondition_ != TC::GROUND) {
        return false;
    }

    auto* comboBehavior = pPlayer->GetComboBehavior();
    if (!comboBehavior) {
        return false;
    }
    const std::string& name = comboBehavior->GetName();
    if (name == "ComboAttackRoot" || name == prevBehaviorName) {
        return false;
    }

    auto* attackData = pAttackController_->GetAttackByName(name);
    if (attackData &&
        attackData->GetAttackParam().triggerParam.condition == TC::AIR &&
        pConditionDataMap_->count(TC::AIR) > 0) {
        isAutoSwitchedCondition_ = true;
        return true;
    }
    return false;
}

///==========================================================
/// 条件切替の実行
///==========================================================
void ComboAsistConditionSwitcher::SwitchCondition(
    PlayerComboAttackData::TriggerCondition condition,
    const std::function<void(ConditionUIData& prev, ConditionUIData& next)>& onSwitch) {

    if (pConditionDataMap_->count(condition) == 0 || condition == *pCurrentCondition_) {
        return;
    }

    ConditionUIData& prevData = (*pConditionDataMap_)[*pCurrentCondition_];
    // 旧条件を非表示
    pUiBuilder_->ApplyToCondition(prevData, [](BaseComboAsistUI& ui) { ui.SetVisible(false); });

    *pCurrentCondition_ = condition;

    ConditionUIData& nextData = (*pConditionDataMap_)[*pCurrentCondition_];
    // コールバック（ApplySlideOffset・SnapToTarget・UpdateVisibility等）
    onSwitch(prevData, nextData);
}

///==========================================================
/// ロック状態の同期
///==========================================================
void ComboAsistConditionSwitcher::SyncUnlockStates() {
    if (!pAttackController_) {
        return;
    }
    for (auto& [cond, data] : *pConditionDataMap_) {
        SyncGroupUnlockStates(data.pathBuilder.GetXGroup(), data.xUIGroup);
        SyncGroupUnlockStates(data.pathBuilder.GetYGroup(), data.yUIGroup);
    }
}

void ComboAsistConditionSwitcher::SyncGroupUnlockStates(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup) {
    pUiBuilder_->ForEachStepButton(pathGroup, uiGroup,
        [&](const ComboPathBuilder::ComboStep& step, ComboAsistButtonUI& btn) {
            auto* attackData = pAttackController_->GetAttackByName(step.attackName);
            if (attackData) {
                btn.SetUnlocked(attackData->GetAttackParam().isUnlocked);
            }
        });
}
