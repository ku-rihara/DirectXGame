#pragma once
// AsistParts
#include "AsistParts/ComboAsistArrowUI.h"
#include "AsistParts/ComboAsistButtonUI.h"
// Builder
#include "ComboPathBuilder.h"
// std
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <vector>

class PlayerComboAttackController;

/// <summary>
/// コンボアシストUI要素のグループ（パスデータはComboPathBuilderが保持）
/// </summary>
struct ComboUIGroup {
    std::vector<std::unique_ptr<ComboAsistButtonUI>> mainButtonUIs;
    std::vector<std::unique_ptr<ComboAsistArrowUI>> mainArrowUIs;

    std::vector<std::vector<std::unique_ptr<ComboAsistButtonUI>>> branchButtonUIs;
    std::vector<std::unique_ptr<ComboAsistArrowUI>> branchArrowUIs;
    std::vector<std::vector<std::unique_ptr<ComboAsistArrowUI>>> branchInnerArrowUIs;

    void Clear() {
        mainButtonUIs.clear();
        mainArrowUIs.clear();
        branchButtonUIs.clear();
        branchArrowUIs.clear();
        branchInnerArrowUIs.clear();
    }
};

/// <summary>
/// 発動条件ごとのUIデータ（パスBuilder + UIグループ x/y）
/// </summary>
struct ConditionUIData {
    ComboPathBuilder pathBuilder;
    ComboUIGroup xUIGroup;
    ComboUIGroup yUIGroup;
    int32_t columnShiftAmount = 0;
    bool isColumnShifting     = false;

    void Clear() {
        xUIGroup.Clear();
        yUIGroup.Clear();
        columnShiftAmount = 0;
        isColumnShifting  = false;
    }

    bool HasData() const {
        return !xUIGroup.mainButtonUIs.empty() || !yUIGroup.mainButtonUIs.empty();
    }
};

/// <summary>
/// コンボアシストUI生成クラス
/// パス情報からUI要素（ボタン・矢印）を生成し、ConditionUIDataに格納する。
/// ヘルパーも提供する。
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
