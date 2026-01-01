#include "ParticleData.h"

using namespace KetaEngine;
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imgui.h>

void ParticleData::InitWithCategory(const std::string& name, const std::string& categoryName) {
    BaseSequenceEffectData::InitWithCategory(name, categoryName);

    groupName_  = name;
    folderPath_ = baseFolderPath_ + categoryName_ + "/" + "Dates";

    if (!globalParameter_->HasRegisters(groupName_)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        GetParams();
    }

    InitParams();
}

void ParticleData::Update(float speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    UpdateKeyFrameProgression();
    UpdateActiveSections(speedRate);
}

void ParticleData::UpdateActiveSections(float speedRate) {
    if (sectionElements_.empty()) {
        return;
    }

    // 全セクションを同時に更新
    for (auto& section : sectionElements_) {
        section->Update(speedRate);
    }
}

void ParticleData::UpdateKeyFrameProgression() {
    if (sectionElements_.empty() || playState_ != PlayState::PLAYING) {
        return;
    }

    // 全セクションが終了したかチェック
    bool allFinished = true;
    for (const auto& section : sectionElements_) {
        if (!section->IsFinished()) {
            allFinished = false;
            break;
        }
    }

    if (allFinished) {
        isAllKeyFramesFinished_ = true;
        playState_              = PlayState::STOPPED;
    }
}

void ParticleData::AdvanceToNexTSequenceElement() {
    if (activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size()) - 1) {
        activeKeyFrameIndex_++;
    }
}

void ParticleData::Reset() {
    for (auto& section : sectionElements_) {
        section->Reset();
    }

    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
    playState_              = PlayState::STOPPED;
}

void ParticleData::Play() {
    BaseEffectData::Play();

    // セクションが空の場合は何もしない
    if (sectionElements_.empty()) {
        return;
    }

    // 全セクションを待機状態で開始
    for (auto& section : sectionElements_) {
        section->StartWaiting();
    }

    // activeKeyFrameIndexをリセット
    activeKeyFrameIndex_ = 0;
}

void ParticleData::RegisterParams() {
}

void ParticleData::GetParams() {
}

void ParticleData::InitParams() {
    playState_              = PlayState::STOPPED;
    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
}

std::unique_ptr<ParticleSection> ParticleData::CreateKeyFrame(int32_t index) {
    auto section = std::make_unique<ParticleSection>();
    section->Init(groupName_, categoryName_, index);
    return section;
}

std::string ParticleData::GeTSequenceElementFolderPath() const {
    return baseFolderPath_ + categoryName_ + "/" + "Sections/" + groupName_ + "/";
}

void ParticleData::AdjustParam() {
#ifdef _DEBUG
    ImGui::Text("Category: %s", categoryName_.c_str());
    ImGui::Text("Particle: %s", groupName_.c_str());

    ImGui::Separator();
    ImGui::Text("Sections: %d", GetTotalKeyFrameCount());

    if (ImGui::Button("Add Section")) {
        AddKeyFrame();
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove Selected") && selectedKeyFrameIndex_ >= 0) {
        RemoveKeyFrame(selectedKeyFrameIndex_);
    }

    // セクションリスト
    for (int i = 0; i < GetTotalKeyFrameCount(); ++i) {
        ImGui::PushID(i);
        bool isSelected = (selectedKeyFrameIndex_ == i);

        std::string labelText = "Section " + std::to_string(i);

        // セクションの状態を表示
        if (i < static_cast<int>(sectionElements_.size()) && sectionElements_[i]) {
            if (sectionElements_[i]->IsPlaying()) {
                labelText += " [PLAYING]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            }
        }

        if (ImGui::Selectable(labelText.c_str(), isSelected)) {
            SetSelectedKeyFrameIndex(i);
        }

        if (i < static_cast<int>(sectionElements_.size()) && sectionElements_[i]) {
            if (sectionElements_[i]->IsPlaying()) {
                ImGui::PopStyleColor();
            }
        }

        ImGui::PopID();
    }

    // 選択されたセクションの編集
    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < GetTotalKeyFrameCount()) {
        ImGui::Separator();
        sectionElements_[selectedKeyFrameIndex_]->AdjustParam();
    }
#endif
}

void ParticleData::LoadSequenceElements() {
    BaseSequenceEffectData::LoadSequenceElements();
}

void ParticleData::SaveSequenceElements() {
    BaseSequenceEffectData::SaveSequenceElements();
}