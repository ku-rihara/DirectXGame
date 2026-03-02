#pragma once
#include "../AsistParts/ComboAsistArrowUI.h"
#include "ComboAsistVisibilityController.h"
#include "../AsistBuilder/ComboUIBuilder.h"
#include "Easing/Easing.h"
#include <cstdint>
#include <string>

/// <summary>
/// コンボアシストの列スクロール・行シフト管理クラス
/// 表示範囲を超えた際の列シフト、分岐発動時の行シフト、リセット処理を担当する
/// </summary>
class ComboAsistColumnScroller {
public:
    ComboAsistColumnScroller()  = default;
    ~ComboAsistColumnScroller() = default;

    void Init(ComboAsistVisibilityController* visibility);

    /// 列オーバーフローの検知・イージング更新を行う
    void CheckColumnOverflow(
        ConditionUIData& currentData,
        bool isVisible);

    /// 分岐攻撃発動時の行シフトを行う
    void CheckRowShift(
        ConditionUIData& currentData,
        bool isVisible,
        const std::string& currentBehaviorName);

    /// コンボリセット時にRow/Columnを初期状態に戻す
    void ResetShifts(ConditionUIData& currentData);

    /// 発動攻撃が表示範囲端に達した際に列シフトを行う（末端判定付き）
    void ShiftColumnsForAttack(
        ConditionUIData& currentData,
        const std::string& attackName,
        int32_t maxVisibleColumn,
        const ComboPathBuilder::ComboPathGroup& xPathGroup,
        const ComboPathBuilder::ComboPathGroup& yPathGroup);

    /// グループの全UI要素を列方向にシフトし、columnShiftAmountを更新する
    void ShiftGroupColumns(ComboUIGroup& uiGroup, int32_t delta);

private:
    void CheckGroupColumnOverflowDetect(ComboUIGroup& uiGroup);
    void ShiftGroup(ComboUIGroup& uiGroup, int32_t delta);

    void ShiftGroupRows(ComboUIGroup& uiGroup, int32_t fromRow, int32_t divergeCol, int32_t delta);
    int32_t FindBranchRowForAttack(const ComboUIGroup& uiGroup, const std::string& attackName) const;
    int32_t FindDivergeColForBranchRow(const ComboUIGroup& uiGroup, int32_t branchRow) const;

    int32_t FindAttackColumnInGroup(const ComboUIGroup& uiGroup, const std::string& attackName) const;
    int32_t GetGroupMaxColumn(const ComboUIGroup& uiGroup) const;
    bool HasNextStepAfterAttack(const ComboPathBuilder::ComboPathGroup& pathGroup, const std::string& attackName) const;

private:
    ComboAsistVisibilityController* pVisibility_ = nullptr;

    // 列オーバーフローイージング
    KetaEngine::Easing<float> columnOverflowEasing_;
    float overflowScale_ = 0.0f;

    int32_t maxVisibleColumn_ = 5;
    int32_t maxVisibleRow_    = 4;

public:
    void SetVisibleRange(int32_t maxCol, int32_t maxRow) {
        maxVisibleColumn_ = maxCol;
        maxVisibleRow_    = maxRow;
    }
};
