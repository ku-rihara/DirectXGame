#pragma once
#include "../AsistBuilder/ComboUIBuilder.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include <map>
#include <string>

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
        PlayerComboAttackData::TriggerCondition* currentCondition);

    /// プレイヤーの状態に基づく自動条件切替を確認する
    /// @param outTargetCondition 切替先の条件（切替が必要な場合のみ設定）
    /// @return 切替が発生したら true
    bool CheckAutoConditionSwitch(
        bool isVisible,
        Player* pPlayer,
        PlayerComboAttackData::TriggerCondition& outTargetCondition);

    /// 全条件のロック状態をAttackControllerと同期する
    void SyncUnlockStates();

    /// 指定した条件にUIを切り替える（表示・スナップ含む）
    /// @param onSwitch 切替時に呼ばれるコールバック（ApplySlideOffset・UpdateVisibility等）
    void SwitchCondition(
        PlayerComboAttackData::TriggerCondition condition,
        const std::function<void(ConditionUIData& prev, ConditionUIData& next)>& onSwitch);


private:
    void SyncGroupUnlockStates(const ComboPathBuilder::ComboPathGroup& pathGroup, ComboUIGroup& uiGroup);

private:
    PlayerComboAttackController* pAttackController_  = nullptr;
    ComboUIBuilder* pUiBuilder_                      = nullptr;
    std::map<PlayerComboAttackData::TriggerCondition, ConditionUIData>* pConditionDataMap_ = nullptr;
    PlayerComboAttackData::TriggerCondition* pCurrentCondition_                            = nullptr;

};
