#include "TimeModeSelector.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imgui.h>

void TimeModeSelector::RegisterParam(const std::string& groupName, GlobalParameter* globalParameter) {
    globalParameter->Regist(groupName, "timeMode", &timeModeInt_);
}

void TimeModeSelector::GetParam(const std::string& groupName, GlobalParameter* globalParameter) {
    timeModeInt_ = globalParameter->GetValue<int32_t>(groupName, "timeMode");
}

void TimeModeSelector::SelectTimeModeImGui(const char* label) {
    int mode = static_cast<int>(timeModeInt_);
    if (ImGui::Combo(label, &mode, TimeModeLabels.data(), static_cast<int>(TimeModeLabels.size()))) {
        timeModeInt_ = mode;
    }
}
