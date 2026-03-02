#pragma once
#include "../AsistBuilder/ComboUIBuilder.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include <map>
#include <string>
#include <vector>

class Player;
class PlayerComboAttackController;

/// <summary>
/// コンボアシストの発動条件切替クラス
/// GROUND/AIR/DASHの手動・自動切替、ロック状態の同期を担当する
/// </summary>
class ComboAsistConditionSwitcher {
public:
    ComboAsistConditionSwitcher()  = default;
    ~ComboAsistConditionSwitcher() = default;

    void Init(
        PlayerComboAttackController* attackController,
        ComboUIBuilder* uiBuilder,
        std::map<PlayerComboAttackData::TriggerCondition, ConditionUIData>* conditionDataMap,
        std::vector<PlayerComboAttackData::TriggerCondition>* availableConditions,
        PlayerComboAttackData::TriggerCondition* currentCondition);

    /// 十字キー入力による手動条件切替を確認する
    /// @return 切替が発生したら true
    bool CheckConditionSwitchInput(bool isPanelOpen);

    /// プレイヤーの状態に基づく自動条件切替を確認する
    /// @return 切替が発生したら true
    bool CheckAutoConditionSwitch(
        bool isVisible,
        Player* pPlayer,
        const std::string& prevBehaviorName);

    /// 全条件のロック状態をAttackControllerと同期する
    void SyncUnlockStates();

    /// 指定した条件にUIを切り替える（表示・スナップ含む）
    /// @param onSwitch 切替時に呼ばれるコールバック（ApplySlideOffset・UpdateVisibility等）
    void SwitchCondition(
        PlayerComboAttackData::TriggerCondition condition,
        const std::function<void(ConditionUIData& prev, ConditionUIData& next)>& onSwitch);

    bool IsAutoSwitched() const { return isAutoSwitchedCondition_; }
    void ClearAutoSwitched() { isAutoSwitchedCondition_ = false; }

private:
    void SyncGroupUnlockStates(const ComboPathBuilder::ComboPathGroup& pathGroup, ComboUIGroup& uiGroup);

private:
    PlayerComboAttackController* pAttackController_  = nullptr;
    ComboUIBuilder* pUiBuilder_                      = nullptr;
    std::map<PlayerComboAttackData::TriggerCondition, ConditionUIData>* pConditionDataMap_ = nullptr;
    std::vector<PlayerComboAttackData::TriggerCondition>* pAvailableConditions_            = nullptr;
    PlayerComboAttackData::TriggerCondition* pCurrentCondition_                            = nullptr;

    bool isAutoSwitchedCondition_ = false;
};
