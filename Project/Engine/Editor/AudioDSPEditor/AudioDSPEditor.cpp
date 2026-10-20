#include "AudioDSPEditor.h"

using namespace KetaEngine;
#include "Audio/DSP/IO/IAudioEncoder.h"
#include "Audio/DSP/IO/ISoundRegistry.h"
#include "AudioDSPContext.h"
#include "Panel/IAudioDSPPanel.h"
#include <imgui.h>

AudioDSPEditor::AudioDSPEditor()  = default;
AudioDSPEditor::~AudioDSPEditor() = default;

void AudioDSPEditor::Init(std::unique_ptr<ISoundRegistry> registry, std::unique_ptr<IAudioEncoder> encoder) {
    panels_.clear();
    context_.reset();

    registry_ = std::move(registry);
    encoder_  = std::move(encoder);
    context_  = std::make_unique<AudioDSPContext>(*registry_, *encoder_);
}

void AudioDSPEditor::AddPanel(std::unique_ptr<IAudioDSPPanel> panel) {
    panels_.push_back(std::move(panel));
}

void AudioDSPEditor::EditorUpdate() {
    if (!context_) {
        return;
    }
    if (!ImGui::CollapsingHeader("Audio DSP")) {
        return;
    }
    ImGui::PushID("AudioDSPEditor");

    ImGui::SliderFloat("Volume", &context_->GetPlayVolume(), 0.0f, 1.0f);
    if (!context_->GetStatus().empty()) {
        ImGui::TextWrapped("%s", context_->GetStatus().c_str());
    }

    for (auto& panel : panels_) {
        const ImGuiTreeNodeFlags flags = panel->IsDefaultOpen() ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
        if (ImGui::TreeNodeEx(panel->GetTitle(), flags)) {
            panel->Draw(*context_);
            ImGui::TreePop();
        }
    }

    ImGui::PopID();
}
