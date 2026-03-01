#pragma once
#include "../AsistParts/ComboAsistArrowUI.h"
#include "../AsistBuilder/ComboUIBuilder.h"
#include <cstdint>

/// <summary>
/// コンボアシストの表示範囲管理クラス
/// 各UI要素が可視範囲内かを判定し、ScaleIn/Outアニメーションをトリガーする
/// </summary>
class ComboAsistVisibilityController {
public:
    ComboAsistVisibilityController()  = default;
    ~ComboAsistVisibilityController() = default;

    /// 表示範囲の最大値を設定する
    void SetVisibleRange(int32_t maxCol, int32_t maxRow);

    /// グループ内の全UI要素の表示/非表示を可視範囲に基づいて更新する
    void UpdateGroupVisibility(ComboUIGroup& uiGroup);

    /// 指定列のUI要素を強制的に範囲外（非表示）にする（スクロールアウト時に使用）
    void TriggerLeaveRangeForColumn(ComboUIGroup& uiGroup, int32_t col);

    /// 条件データのX/Y両グループをまとめて更新する
    void UpdateConditionVisibility(ConditionUIData& conditionData);

private:
    bool IsInVisibleRange(int32_t col, int32_t row) const;
    bool IsArrowVisible(const ComboAsistArrowUI& arrow) const;
    void ApplyRangeVisibleToGroup(ComboUIGroup& uiGroup);

private:
    int32_t maxVisibleColumn_ = 5;
    int32_t maxVisibleRow_    = 4;
};
