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

void ParticleData::UpdateKeyFrameProgression() {
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

    for (auto& section : sectionElements_) {
        section->StartWaiting();
    }
}

void ParticleData::Draw() {
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

void ParticleData::RegisterParams() {
    globalParameter_->Regist(groupName_, "DrawAllSections", &drawAllSections_);
}

void ParticleData::GetParams() {
    drawAllSections_ = globalParameter_->GetValue<bool>(groupName_, "DrawAllSections");
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

void ParticleData::LoadSequenceElements() {
    BaseSequenceEffectData::LoadSequenceElements();
}

void ParticleData::SaveSequenceElements() {
    BaseSequenceEffectData::SaveSequenceElements();
}
