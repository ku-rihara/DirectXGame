#include "ComboAsistColumnScroller.h"
#include "Frame/Frame.h"
#include <algorithm>
#include <climits>

void ComboAsistColumnScroller::Init(ComboAsistVisibilityController* visibility) {
    pVisibility_ = visibility;
    columnOverflowEasing_.Init("ComboAsistColumnOverflow.json");
    columnOverflowEasing_.SetAdaptValue(&overflowScale_);
    columnOverflowEasing_.Reset();
}

///==========================================================
/// 列オーバーフロー検知・イージング更新
///==========================================================
void ComboAsistColumnScroller::CheckColumnOverflow(ConditionUIData& currentData, bool isVisible) {
    if (!isVisible) {
        return;
    }

    // イージング中の場合は更新して完了を待つ
    bool anyShifting = currentData.xUIGroup.isColumnShifting || currentData.yUIGroup.isColumnShifting;
    if (anyShifting) {
        columnOverflowEasing_.Update(KetaEngine::Frame::DeltaTime());
        if (columnOverflowEasing_.IsFinished()) {
            if (currentData.xUIGroup.isColumnShifting) {
                currentData.xUIGroup.isColumnShifting = false;
                ShiftGroupColumns(currentData.xUIGroup, -1);
            }
            if (currentData.yUIGroup.isColumnShifting) {
                currentData.yUIGroup.isColumnShifting = false;
                ShiftGroupColumns(currentData.yUIGroup, -1);
            }
        }
        return;
    }

    CheckGroupColumnOverflowDetect(currentData.xUIGroup);
    CheckGroupColumnOverflowDetect(currentData.yUIGroup);
}

void ComboAsistColumnScroller::CheckGroupColumnOverflowDetect(ComboUIGroup& uiGroup) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        if (btn->GetState() != BaseComboAsistUI::AsistState::NONE && btn->GetColumnNum() > maxVisibleColumn_) {
            // 発動攻撃列より先にボタンが存在しない場合はシフト不要
            int32_t maxCol = GetGroupMaxColumn(uiGroup);
            if (maxCol <= btn->GetColumnNum()) {
                return;
            }
            uiGroup.isColumnShifting = true;
            columnOverflowEasing_.Reset();
            columnOverflowEasing_.SetAdaptValue(&overflowScale_);
            if (pVisibility_) {
                pVisibility_->TriggerLeaveRangeForColumn(uiGroup, 0);
            }
            return;
        }
    }
}

///==========================================================
/// 発動攻撃が表示範囲端に達した際の列シフト
///==========================================================
void ComboAsistColumnScroller::ShiftColumnsForAttack(
    ConditionUIData& currentData,
    const std::string& attackName,
    int32_t maxVisibleColumn,
    const ComboPathBuilder::ComboPathGroup& xPathGroup,
    const ComboPathBuilder::ComboPathGroup& yPathGroup) {

    int32_t xActiveCol = FindAttackColumnInGroup(currentData.xUIGroup, attackName);
    if (xActiveCol >= maxVisibleColumn && !currentData.xUIGroup.isColumnShifting) {
        if (HasNextStepAfterAttack(xPathGroup, attackName)) {
            int32_t shiftAmount = xActiveCol - maxVisibleColumn + 1;
            for (int32_t c = 0; c < shiftAmount; ++c) {
                if (pVisibility_) {
                    pVisibility_->TriggerLeaveRangeForColumn(currentData.xUIGroup, c);
                }
            }
            ShiftGroupColumns(currentData.xUIGroup, -shiftAmount);
        }
    }

    int32_t yActiveCol = FindAttackColumnInGroup(currentData.yUIGroup, attackName);
    if (yActiveCol >= maxVisibleColumn && !currentData.yUIGroup.isColumnShifting) {
        if (HasNextStepAfterAttack(yPathGroup, attackName)) {
            int32_t shiftAmount = yActiveCol - maxVisibleColumn + 1;
            for (int32_t c = 0; c < shiftAmount; ++c) {
                if (pVisibility_) {
                    pVisibility_->TriggerLeaveRangeForColumn(currentData.yUIGroup, c);
                }
            }
            ShiftGroupColumns(currentData.yUIGroup, -shiftAmount);
        }
    }
}

///==========================================================
/// コンボリセット時のRow/Columnリセット
///==========================================================
void ComboAsistColumnScroller::ResetShifts(ConditionUIData& currentData) {
    // Rowリセットを先に行う（divColはカラムシフト後の座標で記録されているため、
    // カラムを先に戻すと座標系がずれてしまう）
    if (currentData.rowShiftAmount != 0) {
        int32_t branchRow = currentData.rowShiftAmount;
        int32_t divCol    = currentData.rowShiftDivergeCol;
        int32_t mainRow   = currentData.rowShiftMainRow;
        int32_t delta     = branchRow - mainRow; // 正方向（復元用）

        ComboUIGroup& shiftedGroup = currentData.rowShiftIsXGroup
                                         ? currentData.xUIGroup
                                         : currentData.yUIGroup;
        // Step1: 現在mainRowにいるbranch itemsをbranchRowへ戻す
        ShiftGroupRows(shiftedGroup, mainRow, divCol, delta);
        // Step2: 範囲外にいるmain items（row = mainRow-delta）をmainRowへ戻す
        ShiftGroupRows(shiftedGroup, mainRow - delta, divCol, delta);

        currentData.rowShiftAmount     = 0;
        currentData.rowShiftDivergeCol = -1;
        currentData.rowShiftMainRow    = 0;
    }

    // Columnリセット（rowリセット後に行うことで座標系が一致する）
    if (currentData.xUIGroup.columnShiftAmount != 0) {
        ShiftGroupColumns(currentData.xUIGroup, currentData.xUIGroup.columnShiftAmount);
    }
    if (currentData.yUIGroup.columnShiftAmount != 0) {
        ShiftGroupColumns(currentData.yUIGroup, currentData.yUIGroup.columnShiftAmount);
    }
}

///==========================================================
/// 分岐攻撃発動時の行シフト
///==========================================================
void ComboAsistColumnScroller::CheckRowShift(
    ConditionUIData& currentData,
    bool isVisible,
    const std::string& currentBehaviorName) {

    if (!isVisible || currentData.rowShiftAmount != 0) {
        return;
    }
    if (currentBehaviorName == "ComboAttackRoot") {
        return;
    }

    // どちらのグループの分岐にいるか探す
    ComboUIGroup* affectedGroup = nullptr;
    int32_t branchRow           = FindBranchRowForAttack(currentData.xUIGroup, currentBehaviorName);
    if (branchRow >= 0) {
        affectedGroup               = &currentData.xUIGroup;
        currentData.rowShiftIsXGroup = true;
    } else {
        branchRow = FindBranchRowForAttack(currentData.yUIGroup, currentBehaviorName);
        if (branchRow >= 0) {
            affectedGroup               = &currentData.yUIGroup;
            currentData.rowShiftIsXGroup = false;
        }
    }

    if (!affectedGroup || branchRow <= 0) {
        return;
    }

    int32_t divCol = FindDivergeColForBranchRow(*affectedGroup, branchRow);
    if (divCol < 0) {
        return;
    }

    // このグループのmainRowを取得（col < divCol にいるmainButtonの行）
    int32_t mainRow = 0;
    for (auto& btn : affectedGroup->mainButtonUIs) {
        if (btn->GetColumnNum() < divCol) {
            mainRow = btn->GetRowNum();
            break;
        }
    }

    const int32_t delta = mainRow - branchRow; // 負の値

    // Step1: 競合するmain items（mainRow, col>=divCol）を範囲外へ
    ShiftGroupRows(*affectedGroup, mainRow, divCol, delta);
    // Step2: branch items（branchRow, col>=divCol）をmainRowへ
    ShiftGroupRows(*affectedGroup, branchRow, divCol, delta);

    currentData.rowShiftAmount     = branchRow;
    currentData.rowShiftDivergeCol = divCol;
    currentData.rowShiftMainRow    = mainRow;
}

///==========================================================
/// グループシフト実装
///==========================================================
void ComboAsistColumnScroller::ShiftGroupColumns(ComboUIGroup& uiGroup, int32_t delta) {
    ShiftGroup(uiGroup, delta);
    uiGroup.columnShiftAmount -= delta;
}

void ComboAsistColumnScroller::ShiftGroup(ComboUIGroup& uiGroup, int32_t delta) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        btn->SetRowColumn(btn->GetRowNum(), btn->GetColumnNum() + delta);
        btn->ApplyLayout();
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            btn->SetRowColumn(btn->GetRowNum(), btn->GetColumnNum() + delta);
            btn->ApplyLayout();
        }
    }
    for (auto& arrow : uiGroup.mainArrowUIs) {
        arrow->ShiftColumns(delta);
        arrow->ApplyLayout();
    }
    for (auto& arrow : uiGroup.branchArrowUIs) {
        arrow->ShiftColumns(delta);
        arrow->ApplyLayout();
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            arrow->ShiftColumns(delta);
            arrow->ApplyLayout();
        }
    }
}

void ComboAsistColumnScroller::ShiftGroupRows(
    ComboUIGroup& uiGroup, int32_t fromRow, int32_t divergeCol, int32_t delta) {

    for (auto& btn : uiGroup.mainButtonUIs) {
        if (btn->GetRowNum() == fromRow && btn->GetColumnNum() >= divergeCol) {
            btn->SetRowColumn(btn->GetRowNum() + delta, btn->GetColumnNum());
            btn->ApplyLayout();
        }
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            if (btn->GetRowNum() == fromRow && btn->GetColumnNum() >= divergeCol) {
                btn->SetRowColumn(btn->GetRowNum() + delta, btn->GetColumnNum());
                btn->ApplyLayout();
            }
        }
    }
    for (auto& arrow : uiGroup.mainArrowUIs) {
        if (arrow->GetFromRow() == fromRow && arrow->GetFromCol() >= divergeCol) {
            arrow->ShiftRows(delta);
            arrow->ApplyLayout();
        }
    }
    for (auto& arrow : uiGroup.branchArrowUIs) {
        if (arrow->GetToRow() == fromRow) {
            arrow->ShiftRows(delta);
            arrow->ApplyLayout();
        }
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            if (arrow->GetFromRow() == fromRow) {
                arrow->ShiftRows(delta);
                arrow->ApplyLayout();
            }
        }
    }
}

///==========================================================
/// 検索・判定ヘルパー
///==========================================================
int32_t ComboAsistColumnScroller::FindBranchRowForAttack(
    const ComboUIGroup& uiGroup, const std::string& attackName) const {
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            if (btn->GetAttackName() == attackName) {
                return btn->GetRowNum();
            }
        }
    }
    return -1;
}

int32_t ComboAsistColumnScroller::FindDivergeColForBranchRow(
    const ComboUIGroup& uiGroup, int32_t branchRow) const {
    int32_t minCol = INT32_MAX;
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            if (btn->GetRowNum() == branchRow && btn->GetColumnNum() < minCol) {
                minCol = btn->GetColumnNum();
            }
        }
    }
    return (minCol == INT32_MAX) ? -1 : minCol;
}

int32_t ComboAsistColumnScroller::FindAttackColumnInGroup(
    const ComboUIGroup& uiGroup, const std::string& attackName) const {
    for (const auto& btn : uiGroup.mainButtonUIs) {
        if (btn->GetAttackName() == attackName) return btn->GetColumnNum();
    }
    for (const auto& buttonRow : uiGroup.branchButtonUIs) {
        for (const auto& btn : buttonRow) {
            if (btn->GetAttackName() == attackName) return btn->GetColumnNum();
        }
    }
    return -1;
}

int32_t ComboAsistColumnScroller::GetGroupMaxColumn(const ComboUIGroup& uiGroup) const {
    int32_t maxCol = -1;
    for (const auto& btn : uiGroup.mainButtonUIs) {
        maxCol = (std::max)(maxCol, btn->GetColumnNum());
    }
    for (const auto& buttonRow : uiGroup.branchButtonUIs) {
        for (const auto& btn : buttonRow) {
            maxCol = (std::max)(maxCol, btn->GetColumnNum());
        }
    }
    return maxCol;
}

bool ComboAsistColumnScroller::HasNextStepAfterAttack(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    const std::string& attackName) const {

    const auto& mainSteps = pathGroup.mainPath.steps;
    for (size_t i = 0; i < mainSteps.size(); ++i) {
        if (mainSteps[i].attackName == attackName) {
            return (i + 1) < mainSteps.size();
        }
    }
    for (const auto& branch : pathGroup.branches) {
        const auto& steps = branch.path.steps;
        for (size_t i = 0; i < steps.size(); ++i) {
            if (steps[i].attackName == attackName) {
                return (i + 1) < steps.size();
            }
        }
    }
    return false;
}
