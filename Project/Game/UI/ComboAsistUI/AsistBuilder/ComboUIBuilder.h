#pragma once
// Builder
#include "ConditionUIData.h"
#include "ComboPathBuilder.h"
// std
#include <functional>
#include <map>
#include <vector>

class PlayerComboAttackController;

/// <summary>
/// コンボアシストUI生成クラス
/// </summary>
class ComboUIBuilder {
public:
    ComboUIBuilder()  = default;
    ~ComboUIBuilder() = default;

    /// 全発動条件のUI・パスを構築してマップに格納する
    void BuildAllConditions(
        PlayerComboAttackController* attackController,
        const LayoutParam& layoutParam,
        std::map<PlayerComboAttackData::TriggerCondition, ConditionUIData>& outConditionDataMap,
        std::vector<PlayerComboAttackData::TriggerCondition>& outAvailableConditions,
        PlayerComboAttackData::TriggerCondition& currentCondition);

    //=== ヘルパー ===

    /// グループ内の全UI要素に操作を適用
    void ApplyToGroup(ComboUIGroup& uiGroup, const std::function<void(BaseComboAsistUI&)>& func);

    /// 条件データの x/y 両グループに操作を適用
    void ApplyToCondition(ConditionUIData& conditionData, const std::function<void(BaseComboAsistUI&)>& func);

    /// パスステップとUIボタンのペアに操作を適用
    void ForEachStepButton(
        const ComboPathBuilder::ComboPathGroup& pathGroup,
        ComboUIGroup& uiGroup,
        const std::function<void(const ComboPathBuilder::ComboStep&, ComboAsistButtonUI&)>& func);

private:
    /// 1つの発動条件のUI生成（X/Yグループ両方）
    void CreateConditionUI(ConditionUIData& conditionData, const LayoutParam& layoutParam);

    /// 1グループ分のUI生成（メインパス＋分岐パス）
    void CreateGroupUI(
        const ComboPathBuilder::ComboPathGroup& pathGroup,
        ComboUIGroup& uiGroup,
        int32_t* currentRow,
        const LayoutParam& layoutParam);
};
