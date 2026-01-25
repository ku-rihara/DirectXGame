#include"EasingParameterData.h"
#include"vector2.h"
#include"vector3.h"
#include <imgui.h>

template struct KetaEngine::EasingParameter<float>;
template struct KetaEngine::EasingParameter<Vector2>;
template struct KetaEngine::EasingParameter<Vector3>;


void ImGuiEasingTypeSelector(const char* label, int32_t& target) {
    int type = static_cast<int32_t>(target);
    if (ImGui::Combo(label, &type, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
        target = type;
    }
}