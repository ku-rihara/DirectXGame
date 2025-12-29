#include "GPUParticleEditor.h"

using namespace KetaEngine;
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imgui.h>

void GPUParticleEditor::Init(const std::string& typeName, bool isUseCategory) {
    BaseEffectEditor::Init(typeName, isUseCategory);
}

void GPUParticleEditor::Update(float speedRate) {
    BaseEffectEditor::Update(speedRate);
}

std::unique_ptr<GPUParticleData> GPUParticleEditor::CreateEffectData() {
    return std::make_unique<GPUParticleData>();
}

void GPUParticleEditor::RenderSpecificUI() {
    ImGui::SeparatorText("Particle Control");

    BaseEffectEditor::RenderPlayBack();
}

std::string GPUParticleEditor::GetFolderPath() const {
    return particleFolderName_;
}

void GPUParticleEditor::EditorUpdate() {
    BaseEffectEditor::EditorUpdate();
}

void GPUParticleEditor::PlaySelectedAnimation() {
    BaseEffectEditor::PlaySelectedAnimation();
}
