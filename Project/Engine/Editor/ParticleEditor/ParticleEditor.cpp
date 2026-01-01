#include "ParticleEditor.h"

using namespace KetaEngine;
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imgui.h>

void ParticleEditor::Init(const std::string& typeName, bool isUseCategory) {
    BaseEffectEditor::Init(typeName, isUseCategory);

    // 初期化後、全てのパーティクルグループを生成
    InitializeAllParticleGroups();
}

void ParticleEditor::Update(float speedRate) {
    BaseEffectEditor::Update(speedRate);
}

std::unique_ptr<ParticleData> ParticleEditor::CreateEffectData() {
    return std::make_unique<ParticleData>();
}

void ParticleEditor::InitializeAllParticleGroups() {
    // カテゴリーモードの場合
    for (auto& category : categories_) {
        for (auto& effect : category.effects) {
            auto* particleData = dynamic_cast<ParticleData*>(effect.get());
            if (particleData) {
                InitializeParticleDataGroups(particleData);
            }
        }
    }
}

void ParticleEditor::InitializeParticleDataGroups(ParticleData* particleData) {
    if (!particleData) {
        return;
    }

    auto& sections = particleData->GetSectionElements();
    for (auto& section : sections) {
        InitializeSectionParticleGroup(section.get());
    }
}

void ParticleEditor::InitializeSectionParticleGroup(ParticleSection* section) {
    if (!section) {
        return;
    }

    // モデルまたはプリミティブでパーティクルグループを作成
    if (section->GetSectionParam()->IsUseModel()) {
        section->CreateModelParticle(
            section->GetSectionParam()->GetModelFilePath(),
            section->GetSectionParam()->GetMaxParticleNum());
    } else {
        section->CreatePrimitiveParticle(
            static_cast<PrimitiveType>(section->GetSectionParam()->GetPrimitiveTypeInt()),
            section->GetSectionParam()->GetMaxParticleNum());
    }

    // テクスチャを適用
    section->InitAdaptTexture();
}

void ParticleEditor::RenderSpecificUI() {
    ImGui::SeparatorText("Particle Control");

    // エディタ専用のプレビュー機能
    ImGui::Separator();
    ImGui::Text("Editor Preview");

    auto* selectedEffectData = GetSelectedEffect();
    if (selectedEffectData) {
        auto* particleData = dynamic_cast<ParticleData*>(selectedEffectData);
        if (particleData && particleData->GetTotalKeyFrameCount() > 0) {
            int selectedSectionIndex = particleData->GetSelectedKeyFrameIndex();

            // 全体のプレビューコントロール
            ImGui::Text("All Sections Control:");

            if (ImGui::Button("Preview All Sections")) {
                PreviewAllSections(particleData);
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop All")) {
                StopAllSections(particleData);
            }

            // 全体の状態表示
            bool anyPlaying = false;
            auto& sections  = particleData->GetSectionElements();
            for (const auto& section : sections) {
                if (section->IsPlaying()) {
                    anyPlaying = true;
                    break;
                }
            }

            ImGui::SameLine();
            if (anyPlaying) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[PLAYING]");
            } else {
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[STOPPED]");
            }

            ImGui::Separator();

            // 選択中のセクションのプレビュー
            if (selectedSectionIndex >= 0 && selectedSectionIndex < particleData->GetTotalKeyFrameCount()) {
                auto* selectedSection = sections[selectedSectionIndex].get();

                ImGui::Text("Selected Section: %s", selectedSection->GetGroupName().c_str());

                ImGui::SameLine();
                if (ImGui::Button("Stop Preview")) {
                    selectedSection->Stop();
                }

                // セクションの状態表示
                ImGui::SameLine();
                switch (selectedSection->GetPlayState()) {
                case ParticleSection::PlayState::STOPPED:
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[STOPPED]");
                    break;
                case ParticleSection::PlayState::WAITING:
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[WAITING]");
                    break;
                case ParticleSection::PlayState::PLAYING:
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[PLAYING]");
                    break;
                }
            }
        } else {
            ImGui::Text("No sections available");
            if (ImGui::Button("Add First Section")) {
                particleData->AddKeyFrame();
            }
        }
    }
}

void ParticleEditor::PreviewAllSections(ParticleData* particleData) {
    if (!particleData)
        return;

    auto& sections = particleData->GetSectionElements();

    // セクションが空の場合は何もしない
    if (sections.empty()) {
        return;
    }

    particleData->Play();
}

void ParticleEditor::StopAllSections(ParticleData* particleData) {
    if (!particleData)
        return;

    particleData->Pause();
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