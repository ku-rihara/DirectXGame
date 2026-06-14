#pragma once
// Builder
#include "ComboPathBuilder.h"
#include "ComboUIGroup.h"
// std
#include <cstdint>

/// <summary>
/// 発動条件ごとのUIデータ
/// </summary>
struct ConditionUIData {
    ComboPathBuilder pathBuilder;
    ComboUIGroup xUIGroup;
    ComboUIGroup yUIGroup;
    int32_t rowShiftAmount     = 0;    // シフト元のbranchRow
    int32_t rowShiftDivergeCol = -1;   // シフト発動時の分岐列
    int32_t rowShiftMainRow    = 0;    // シフト発動時のmainRow
    bool rowShiftIsXGroup      = true; // シフトしたのはXグループか

    void Clear();
    bool HasData() const;
};
