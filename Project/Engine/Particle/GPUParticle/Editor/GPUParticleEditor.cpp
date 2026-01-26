#include "GPUParticleEditor.h"

using namespace KetaEngine;
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imgui.h>

void GPUParticleEditor::Init(const std::string& typeName) {
    BaseEffectEditor::Init(typeName);
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

std::string GPUParticleEditor::GetFolderName() const {
    return particleFolderName_;
}

void GPUParticleEditor::PlaySelectedAnimation() {
    BaseEffectEditor::PlaySelectedAnimation();
}
