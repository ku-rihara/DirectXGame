#pragma once

#include <imgui.h>
#include <string>

namespace KetaEngine {

class ISoundRegistry;

/// <summary>
/// AudioDSPエディター共通のImGui部品
/// </summary>
class AudioDSPGui {
public:
    /// <summary>
    /// enumをComboで選択(EnumにCountが必要)
    /// </summary>
    /// <param name="nameFunc">enum値 → 表示名</param>
    template <class E, class NameFunc>
    bool EnumCombo(const char* label, E& value, NameFunc nameFunc) const;

    bool DragDouble(const char* label, double& value, float speed, double minValue, double maxValue, const char* format = "%.2f") const;

    // 登録済みサウンドから選択
    bool SoundCombo(const char* label, std::string& selected, const ISoundRegistry& registry) const;
};

template <class E, class NameFunc>
bool AudioDSPGui::EnumCombo(const char* label, E& value, NameFunc nameFunc) const {
    bool changed = false;
    if (ImGui::BeginCombo(label, nameFunc(value))) {
        for (int i = 0; i < static_cast<int>(E::Count); ++i) {
            const E item = static_cast<E>(i);
            if (ImGui::Selectable(nameFunc(item), item == value)) {
                value   = item;
                changed = true;
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}

}; // KetaEngine
