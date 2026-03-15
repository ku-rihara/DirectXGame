#include "ComboAsistVisibilityController.h"

void ComboAsistVisibilityController::SetVisibleRange(int32_t maxCol, int32_t maxRow) {
    maxVisibleColumn_ = maxCol;
    maxVisibleRow_    = maxRow;
}

///==========================================================
/// 条件データのX/Y両グループをまとめて更新
///==========================================================
void ComboAsistVisibilityController::UpdateConditionVisibility(ConditionUIData& conditionData) {
    UpdateGroupVisibility(conditionData.xUIGroup);
    UpdateGroupVisibility(conditionData.yUIGroup);
}

void ComboAsistVisibilityController::SnapConditionVisibility(ConditionUIData& conditionData) {
    SnapGroupVisibility(conditionData.xUIGroup);
    SnapGroupVisibility(conditionData.yUIGroup);
}

///==========================================================
/// グループ内の全UI要素の表示/非表示を可視範囲に基づいて更新
///==========================================================
void ComboAsistVisibilityController::UpdateGroupVisibility(ComboUIGroup& uiGroup) {
    ApplyRangeVisibleToGroup(uiGroup);
}

void ComboAsistVisibilityController::SnapGroupVisibility(ComboUIGroup& uiGroup) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        btn->SnapRangeState(IsInVisibleRange(btn->GetColumnNum(), btn->GetRowNum()));
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            btn->SnapRangeState(IsInVisibleRange(btn->GetColumnNum(), btn->GetRowNum()));
        }
    }
    for (auto& arrow : uiGroup.mainArrowUIs) {
        arrow->SnapRangeState(IsArrowVisible(*arrow));
    }
    for (auto& arrow : uiGroup.branchArrowUIs) {
        arrow->SnapRangeState(IsArrowVisible(*arrow));
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            arrow->SnapRangeState(IsArrowVisible(*arrow));
        }
    }
}

void ComboAsistVisibilityController::ApplyRangeVisibleToGroup(ComboUIGroup& uiGroup) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        btn->SetRangeVisible(IsInVisibleRange(btn->GetColumnNum(), btn->GetRowNum()));
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            btn->SetRangeVisible(IsInVisibleRange(btn->GetColumnNum(), btn->GetRowNum()));
        }
    }
    for (auto& arrow : uiGroup.mainArrowUIs) {
        arrow->SetRangeVisible(IsArrowVisible(*arrow));
    }
    for (auto& arrow : uiGroup.branchArrowUIs) {
        arrow->SetRangeVisible(IsArrowVisible(*arrow));
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            arrow->SetRangeVisible(IsArrowVisible(*arrow));
        }
    }
}

///==========================================================
/// 指定列のUI要素を強制的に範囲外にする
///==========================================================
void ComboAsistVisibilityController::TriggerLeaveRangeForColumn(ComboUIGroup& uiGroup, int32_t col) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        if (btn->GetColumnNum() == col) {
            btn->SetRangeVisible(false);
        }
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            if (btn->GetColumnNum() == col) {
                btn->SetRangeVisible(false);
            }
        }
    }
    for (auto& arrow : uiGroup.mainArrowUIs) {
        if (arrow->GetFromCol() == col) {
            arrow->SetRangeVisible(false);
        }
    }
    for (auto& arrow : uiGroup.branchArrowUIs) {
        if (arrow->GetFromCol() == col) {
            arrow->SetRangeVisible(false);
        }
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            if (arrow->GetFromCol() == col) {
                arrow->SetRangeVisible(false);
            }
        }
    }
}

///==========================================================
/// 範囲判定ヘルパー
///==========================================================
bool ComboAsistVisibilityController::IsInVisibleRange(int32_t col, int32_t row) const {
    return col >= 0 && col <= maxVisibleColumn_ && row >= 0 && row <= maxVisibleRow_;
}

bool ComboAsistVisibilityController::IsArrowVisible(const ComboAsistArrowUI& arrow) const {
    return arrow.GetFromCol() >= 0 && arrow.GetToCol() <= maxVisibleColumn_ &&
           arrow.GetFromRow() >= 0 && arrow.GetToRow() <= maxVisibleRow_;
}
