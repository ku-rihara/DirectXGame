#include "PostEffectEditor.h"

using namespace KetaEngine;
#include <imgui.h>

void PostEffectEditor::Init(const std::string& typeName) {
    BaseEffectEditor::Init(typeName);
}

std::unique_ptr<PostEffectData> PostEffectEditor::CreateEffectData() {
    return std::make_unique<PostEffectData>();
}

void PostEffectEditor::RenderSpecificUI() {
    ImGui::SeparatorText("PostEffect Editor");
    BaseEffectEditor::RenderPlayBack();
}

void PostEffectEditor::PlaySelectedAnimation() {
    auto* selected = GetSelectedEffect();
    if (selected) {
        selected->Play();
    }
}

std::string PostEffectEditor::GetFolderName() const {
    return "PostEffectEditor/";
}

void PostEffectEditor::PlayPostEffect(const std::string& effectName, const std::string& categoryName) {
    auto* effect = GetEffectByName(categoryName, effectName);
    if (effect) {
        effect->Play();
    }
}
