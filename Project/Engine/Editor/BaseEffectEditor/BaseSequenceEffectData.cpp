#include "BaseSequenceEffectData.h"

using namespace KetaEngine;
// editor
#include "Editor/CameraEditor/CameraKeyFrame.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationSection.h"
#include "Editor/RailEditor/RailControlPoint.h"
#include"Particle/GPUParticle/Editor/GPUParticleSection.h"
#include"Particle/CPUParticle/Editor/ParticleSection.h"
// std
#include <algorithm>
#include <filesystem>

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::Init(const std::string& name) {
    groupName_       = name;
    globalParameter_ = GlobalParameter::GetInstance();
    categoryName_    = "";
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::InitWithCategory(const std::string& name, const std::string& categoryName) {
    groupName_       = name;
    categoryName_    = categoryName;
    globalParameter_ = GlobalParameter::GetInstance();
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::LoadData() {
    // まず基底クラスのパラメータをロード
    if (!folderPath_.empty()) {
        globalParameter_->LoadFile(groupName_, folderPath_);
        globalParameter_->SyncParamForGroup(groupName_);
        GetParams();
    }

    // 次にシーケンス要素をロード
    LoadSequenceElements();
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::SaveData() {
    // 基底クラスのパラメータを保存
    if (!folderPath_.empty()) {
        globalParameter_->SaveFile(groupName_, folderPath_);
    }

    // シーケンス要素を保存
    SaveSequenceElements();
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::AddKeyFrame() {
    int32_t newIndex = static_cast<int32_t>(sectionElements_.size());
    auto newKeyFrame = CreateKeyFrame(newIndex);
    sectionElements_.push_back(std::move(newKeyFrame));
    selectedKeyFrameIndex_  = newIndex;
    isAllKeyFramesFinished_ = false;
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::RemoveKeyFrame(int32_t index) {
    if (index >= 0 && index < static_cast<int32_t>(sectionElements_.size())) {
        sectionElements_.erase(sectionElements_.begin() + index);

        if (selectedKeyFrameIndex_ >= index) {
            selectedKeyFrameIndex_--;
            if (selectedKeyFrameIndex_ < 0 && !sectionElements_.empty()) {
                selectedKeyFrameIndex_ = 0;
            }
        }

        if (activeKeyFrameIndex_ >= index) {
            activeKeyFrameIndex_--;
            if (activeKeyFrameIndex_ < 0 && !sectionElements_.empty()) {
                activeKeyFrameIndex_ = 0;
            }
        }

        UpdateKeyFrameIndices();
        isAllKeyFramesFinished_ = false;
    }
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::ClearKeyFrames() {
    sectionElements_.clear();
    selectedKeyFrameIndex_  = -1;
    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::UpdateKeyFrameIndices() {
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::LoadSequenceElements() {
    std::string folderPath = globalParameter_->GetDirectoryPath() + GeTSequenceElementFolderPath();

    if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) {
        return;
    }

    std::vector<std::pair<int32_t, std::string>> keyFrameFiles;

    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::string fileName = entry.path().stem().string();

            if (fileName.find(groupName_) == 0) {
                std::string indexStr = fileName.substr(groupName_.length());

                int32_t index = std::stoi(indexStr);
                keyFrameFiles.emplace_back(index, fileName);
            }
        }
    }

    std::sort(keyFrameFiles.begin(), keyFrameFiles.end());

    if (sectionElements_.size() == 0) {
        ClearKeyFrames();
        for (const auto& [index, fileName] : keyFrameFiles) {
            auto newKeyFrame = CreateKeyFrame(index);
            newKeyFrame->LoadData();
            sectionElements_.push_back(std::move(newKeyFrame));
        }
    } else {
        for (auto& keyFrame : sectionElements_) {
            keyFrame->LoadData();
        }
    }
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::SaveSequenceElements() {
    for (auto& keyFrame : sectionElements_) {
        keyFrame->SaveData();
    }
}

template <typename TSequenceElement>
const TSequenceElement* BaseSequenceEffectData<TSequenceElement>::GetSelectedKeyFrame() const {
    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        return sectionElements_[selectedKeyFrameIndex_].get();
    }
    return nullptr;
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::SetSelectedKeyFrameIndex(int32_t index) {
    if (index >= -1 && index < static_cast<int32_t>(sectionElements_.size())) {
        selectedKeyFrameIndex_ = index;
    }
}

// 明示的なインスタンス化
template class BaseSequenceEffectData<CameraKeyFrame>;
template class BaseSequenceEffectData<ObjEaseAnimationSection>;
template class BaseSequenceEffectData<RailControlPoint>;
template class BaseSequenceEffectData<GPUParticleSection>;
template class BaseSequenceEffectData<ParticleSection>;
