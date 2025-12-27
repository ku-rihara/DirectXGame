#include "ParticleEditor.h"

using namespace KetaEngine;
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imgui.h>

void ParticleEditor::Init(const std::string& typeName, bool isUseCategory) {
    BaseEffectEditor::Init(typeName, isUseCategory);
}

void ParticleEditor::Update(float speedRate) {
    BaseEffectEditor::Update(speedRate);
}

std::unique_ptr<ParticleData> ParticleEditor::CreateEffectData() {
    return std::make_unique<ParticleData>();
}

void ParticleEditor::RenderSpecificUI() {
    ImGui::SeparatorText("Particle Control");

    BaseEffectEditor::RenderPlayBack();
}

std::string ParticleEditor::GetFolderPath() const {
    return particleFolderName_;
}

void ParticleEditor::EditorUpdate() {
    BaseEffectEditor::EditorUpdate();
}

void ParticleEditor::PlaySelectedAnimation() {
    BaseEffectEditor::PlaySelectedAnimation();
}
