#include "TimeScaleEditor.h"

using namespace KetaEngine;
#include <imgui.h>

void TimeScaleEditor::Init(const std::string& editorName, bool isUseCategory) {
    BaseEffectEditor::Init(editorName, isUseCategory);
}

void TimeScaleEditor::Update(float speedRate) {
    // すべてのTimeScaleを更新
    for (auto& timeScale : effects_) {
        timeScale->Update(speedRate);
    }
}

void TimeScaleEditor::EditorUpdate() {
    BaseEffectEditor::EditorUpdate();
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

std::string TimeScaleEditor::GetFolderPath() const {
    return folderName_;
}

void TimeScaleEditor::RenderSpecificUI() {
    ImGui::SeparatorText("TimeScale Editor Settings");

    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        ImGui::Separator();
        BaseEffectEditor::RenderPlayBack();
    }
}