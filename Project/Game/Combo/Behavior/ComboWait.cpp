#include "ComboWait.h"
#include "Combo/ComboUIController.h"
/// frame
#include "Frame/Frame.h"
/// input
#include "JoyState/JoyState.h"
/// imGui
#include <imgui.h>

// 初期化
ComboWait::ComboWait(ComboUIController* uiController)
    : BaseComboUIBehavior("ComboWait", uiController) {
}

ComboWait ::~ComboWait() {
}

// 更新
void ComboWait::Update() {
}

void ComboWait::Debug() {
    ImGui::Text("ComboWait");
}
