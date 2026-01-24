#include "GPUParticleData.h"

using namespace KetaEngine;
// manager
#include "Particle/GPUParticle/GPUParticleManager.h"
// std
#include <algorithm>
#include <filesystem>
// imGui
#include <imgui.h>

void GPUParticleData::InitWithCategory(const std::string& name, const std::string& categoryName) {
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

void GPUParticleData::Update(float speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    UpdateKeyFrameProgression();
    UpdateActiveSections(speedRate);
}

void GPUParticleData::UpdateActiveSections(float speedRate) {
    if (sectionElements_.empty()) {
        return;
    }

    if (drawAllSections_) {
        // 全セクションを同時に更新
        for (auto& section : sectionElements_) {
            section->Update(speedRate);
        }
    } else {
        // アクティブなセクションのみ更新
        if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
            sectionElements_[activeKeyFrameIndex_]->Update(speedRate);
        }
    }
}

void GPUParticleData::UpdateKeyFrameProgression() {
    if (sectionElements_.empty() || playState_ != PlayState::PLAYING) {
        return;
    }

    // 全セクション同時再生モードの場合は進行管理をスキップ
    if (drawAllSections_) {
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
        return;
    }

    // 順次再生モードの場合
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        if (!sectionElements_[activeKeyFrameIndex_]->IsFinished()) {
            return;
        }

        if (activeKeyFrameIndex_ == static_cast<int32_t>(sectionElements_.size()) - 1) {
            isAllKeyFramesFinished_ = true;
            playState_              = PlayState::STOPPED;
        } else {
            AdvanceToNexTSequenceElement();
        }
    }
}

void GPUParticleData::AdvanceToNexTSequenceElement() {
    if (activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size()) - 1) {
        activeKeyFrameIndex_++;
    }
}

void GPUParticleData::Reset() {
    for (auto& section : sectionElements_) {
        section->Reset();
    }

    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
    playState_              = PlayState::STOPPED;
}

void GPUParticleData::Play() {
    BaseEffectData::Play();

    for (auto& section : sectionElements_) {
        section->StartWaiting();
    }
}

void GPUParticleData::Draw() {
    if (sectionElements_.empty()) {
        return;
    }

    if (drawAllSections_) {
        // 全セクションを描画
        for (auto& section : sectionElements_) {
            section->Emit();
        }
    } else {
        // アクティブなセクションのみ描画
        if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
            sectionElements_[activeKeyFrameIndex_]->Emit();
        }
    }
}

void GPUParticleData::RegisterParams() {
    globalParameter_->Regist(groupName_, "DrawAllSections", &drawAllSections_);
}

void GPUParticleData::GetParams() {
    drawAllSections_ = globalParameter_->GetValue<bool>(groupName_, "DrawAllSections");
}

void GPUParticleData::InitParams() {
    playState_              = PlayState::STOPPED;
    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
}

std::unique_ptr<GPUParticleSection> GPUParticleData::CreateKeyFrame(int32_t index) {
    auto section = std::make_unique<GPUParticleSection>();
    section->Init(groupName_, categoryName_, index);
    return section;
}

std::string GPUParticleData::GeTSequenceElementFolderPath() const {
    return baseFolderPath_ + categoryName_ + "/" + "Sections/" + groupName_ + "/";
}

void GPUParticleData::AdjustParam() {
#ifdef _DEBUG
    ImGui::Text("Category: %s", categoryName_.c_str());
    ImGui::Text("Particle: %s", groupName_.c_str());

    ImGui::Checkbox("Draw All Sections", &drawAllSections_);

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
        if (ImGui::Selectable(("Section " + std::to_string(i)).c_str(), isSelected)) {
            SetSelectedKeyFrameIndex(i);
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

void GPUParticleData::LoadSequenceElements() {
    BaseSequenceEffectData::LoadSequenceElements();
}

void GPUParticleData::SaveSequenceElements() {
    BaseSequenceEffectData::SaveSequenceElements();
}

GPUParticleSection* GPUParticleData::GetKeyFrame(int32_t index) {
    if (index >= 0 && index < static_cast<int32_t>(sectionElements_.size())) {
        return sectionElements_[index].get();
    }
    return nullptr;
}
