#pragma once
// AssistParts
#include "../AssistParts/ComboAssistArrowUI.h"
#include "../AssistParts/ComboAssistButtonUI.h"
// std
#include <cstdint>
#include <memory>
#include <vector>

/// <summary>
/// コンボアシストUI要素のグループ
/// </summary>
struct ComboUIGroup {
    std::vector<std::unique_ptr<ComboAssistButtonUI>> mainButtonUIs;
    std::vector<std::unique_ptr<ComboAssistArrowUI>> mainArrowUIs;

    std::vector<std::vector<std::unique_ptr<ComboAssistButtonUI>>> branchButtonUIs;
    std::vector<std::unique_ptr<ComboAssistArrowUI>> branchArrowUIs;
    std::vector<std::vector<std::unique_ptr<ComboAssistArrowUI>>> branchInnerArrowUIs;

    // グループ独立のカラムシフト状態
    int32_t columnShiftAmount = 0;
    bool isColumnShifting     = false;

    void Clear();
};
