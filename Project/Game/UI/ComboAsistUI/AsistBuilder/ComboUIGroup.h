#pragma once
// AsistParts
#include "../AsistParts/ComboAsistArrowUI.h"
#include "../AsistParts/ComboAsistButtonUI.h"
// std
#include <cstdint>
#include <memory>
#include <vector>

/// <summary>
/// コンボアシストUI要素のグループ
/// </summary>
struct ComboUIGroup {
    std::vector<std::unique_ptr<ComboAsistButtonUI>> mainButtonUIs;
    std::vector<std::unique_ptr<ComboAsistArrowUI>> mainArrowUIs;

    std::vector<std::vector<std::unique_ptr<ComboAsistButtonUI>>> branchButtonUIs;
    std::vector<std::unique_ptr<ComboAsistArrowUI>> branchArrowUIs;
    std::vector<std::vector<std::unique_ptr<ComboAsistArrowUI>>> branchInnerArrowUIs;

    // グループ独立のカラムシフト状態
    int32_t columnShiftAmount = 0;
    bool isColumnShifting     = false;

    void Clear();
};
