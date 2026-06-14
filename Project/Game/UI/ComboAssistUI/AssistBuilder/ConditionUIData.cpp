#include "ConditionUIData.h"

void ConditionUIData::Clear() {
    xUIGroup.Clear();
    yUIGroup.Clear();
    rowShiftAmount     = 0;
    rowShiftDivergeCol = -1;
    rowShiftMainRow    = 0;
    rowShiftIsXGroup   = true;
}

bool ConditionUIData::HasData() const {
    return !xUIGroup.mainButtonUIs.empty() || !yUIGroup.mainButtonUIs.empty();
}
