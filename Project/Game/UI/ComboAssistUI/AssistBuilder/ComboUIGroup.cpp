#include "ComboUIGroup.h"

void ComboUIGroup::Clear() {
    mainButtonUIs.clear();
    mainArrowUIs.clear();
    branchButtonUIs.clear();
    branchArrowUIs.clear();
    branchInnerArrowUIs.clear();
    columnShiftAmount = 0;
    isColumnShifting  = false;
}
