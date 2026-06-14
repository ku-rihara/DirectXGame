#include "ComboAssistConditionSwitcher.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"

void ComboAssistConditionSwitcher::Init(
    PlayerComboAttackController* attackController,
    ComboUIBuilder* uiBuilder,
    std::map<PlayerComboAttackData::TriggerCondition, ConditionUIData>* conditionDataMap,
    PlayerComboAttackData::TriggerCondition* currentCondition) {

    pAttackController_ = attackController;
    pUiBuilder_        = uiBuilder;
    pConditionDataMap_ = conditionDataMap;
    pCurrentCondition_ = currentCondition;
}


///==========================================================
/// 自動条件切替
///==========================================================
bool ComboAssistConditionSwitcher::CheckAutoConditionSwitch(
    bool isVisible,
    Player* pPlayer,
    PlayerComboAttackData::TriggerCondition& outTargetCondition) {

    if (!isVisible || !pPlayer) {
        return false;
    }

    using TC = PlayerComboAttackData::TriggerCondition;
    TC desired = TC::GROUND;

    if (pPlayer->GetBehaviors().IsAirborne() && pConditionDataMap_->count(TC::AIR) > 0) {
        desired = TC::AIR;
    } else if (pPlayer->GetBehaviors().IsDashing() && pConditionDataMap_->count(TC::DASH) > 0) {
        desired = TC::DASH;
    }

    if (desired == *pCurrentCondition_) {
        return false;
    }

    outTargetCondition = desired;
    return true;
}

///==========================================================
/// 条件切替の実行
///==========================================================
void ComboAssistConditionSwitcher::SwitchCondition(
    PlayerComboAttackData::TriggerCondition condition,
    const std::function<void(ConditionUIData& prev, ConditionUIData& next)>& onSwitch) {

    if (pConditionDataMap_->count(condition) == 0 || condition == *pCurrentCondition_) {
        return;
    }

    ConditionUIData& prevData = (*pConditionDataMap_)[*pCurrentCondition_];
    // 旧条件を非表示
    pUiBuilder_->ApplyToCondition(prevData, [](BaseComboAssistUI& ui) { ui.SetVisible(false); });

    *pCurrentCondition_ = condition;

    ConditionUIData& nextData = (*pConditionDataMap_)[*pCurrentCondition_];
    // コールバック（ApplySlideOffset・SnapToTarget・UpdateVisibility等）
    onSwitch(prevData, nextData);
}

///==========================================================
/// ロック状態の同期
///==========================================================
void ComboAssistConditionSwitcher::SyncUnlockStates() {
    if (!pAttackController_) {
        return;
    }
    for (auto& [cond, data] : *pConditionDataMap_) {
        SyncGroupUnlockStates(data.pathBuilder.GetXGroup(), data.xUIGroup);
        SyncGroupUnlockStates(data.pathBuilder.GetYGroup(), data.yUIGroup);
    }
}

void ComboAssistConditionSwitcher::SyncGroupUnlockStates(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup) {
    pUiBuilder_->ForEachStepButton(pathGroup, uiGroup,
        [&](const ComboPathBuilder::ComboStep& step, ComboAssistButtonUI& btn) {
            auto* attackData = pAttackController_->GetAttackByName(step.attackName);
            if (attackData) {
                btn.SetUnlocked(attackData->GetAttackParam().isUnlocked);
            }
        });
}
