#include "TimeScaleEditor.h"

using namespace KetaEngine;
#include <imgui.h>

void TimeScaleEditor::Init(const std::string& typeName) {
    BaseEffectEditor::Init(typeName);
}

void TimeScaleEditor::Update(float deltaTime) {
    // 基底クラスのUpdate呼び出し
    BaseEffectEditor::Update(deltaTime);
}

void TimeScaleEditor::PlaySelectedAnimation() {
    auto* selectedTimeScale = GetSelectedEffect();
    if (selectedTimeScale) {
        selectedTimeScale->Play();
    }
}

std::unique_ptr<TimeScaleData> TimeScaleEditor::CreateEffectData() {
    return std::make_unique<TimeScaleData>();
}

std::string TimeScaleEditor::GetFolderName() const {
    return "TimeScale/";
}

void TimeScaleEditor::RenderSpecificUI() {
    ImGui::SeparatorText("TimeScale Editor Settings");

    ImGui::Separator();
    BaseEffectEditor::RenderPlayBack();
}