#include "AudioDSPGui.h"

using namespace KetaEngine;
#include "Audio/DSP/IO/ISoundRegistry.h"

bool AudioDSPGui::DragDouble(const char* label, double& value, float speed, double minValue, double maxValue, const char* format) const {
    return ImGui::DragScalar(label, ImGuiDataType_Double, &value, speed, &minValue, &maxValue, format);
}

bool AudioDSPGui::SoundCombo(const char* label, std::string& selected, const ISoundRegistry& registry) const {
    bool changed = false;
    if (ImGui::BeginCombo(label, selected.empty() ? "(none)" : selected.c_str())) {
        for (const auto& name : registry.GetSoundNames()) {
            if (ImGui::Selectable(name.c_str(), name == selected)) {
                selected = name;
                changed  = true;
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}
