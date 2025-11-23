#include "BaseSequenceEffectData.h"
#include "Editor/CameraEditor/CameraKeyFrame.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationSection.h"
#include "Editor/RailEditor/RailControlPoint.h"
#include <algorithm>
#include <filesystem>

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::Init(const std::string& name) {
    groupName_       = name;
    globalParameter_ = GlobalParameter::GetInstance();
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::InitWithCategory(const std::string& name, const std::string& categoryName) {
    groupName_       = name;
    categoryName;
    globalParameter_ = GlobalParameter::GetInstance();
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::AddKeyFrame() {
    int32_t newIndex = static_cast<int32_t>(keyFrames_.size());
    auto newKeyFrame = CreateKeyFrame(newIndex);
    keyFrames_.push_back(std::move(newKeyFrame));
    selectedKeyFrameIndex_  = newIndex;
    isAllKeyFramesFinished_ = false;
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::RemoveKeyFrame(const int32_t& index) {
    if (index >= 0 && index < static_cast<int32_t>(keyFrames_.size())) {
        keyFrames_.erase(keyFrames_.begin() + index);

        // インデックス調整
        if (selectedKeyFrameIndex_ >= index) {
            selectedKeyFrameIndex_--;
            if (selectedKeyFrameIndex_ < 0 && !keyFrames_.empty()) {
                selectedKeyFrameIndex_ = 0;
            }
        }

        if (activeKeyFrameIndex_ >= index) {
            activeKeyFrameIndex_--;
            if (activeKeyFrameIndex_ < 0 && !keyFrames_.empty()) {
                activeKeyFrameIndex_ = 0;
            }
        }

        // インデックスの再設定
        UpdateKeyFrameIndices();
        isAllKeyFramesFinished_ = false;
    }
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::ClearKeyFrames() {
    keyFrames_.clear();
    selectedKeyFrameIndex_  = -1;
    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::UpdateKeyFrameIndices() {
   
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::LoadKeyFrames() {
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
                int32_t index        = std::stoi(indexStr);
                keyFrameFiles.emplace_back(index, fileName);
            }
        }
    }

    std::sort(keyFrameFiles.begin(), keyFrameFiles.end());

    if (keyFrames_.size() == 0) {
        ClearKeyFrames();
        for (const auto& [index, fileName] : keyFrameFiles) {
            auto newKeyFrame = CreateKeyFrame(index);
            newKeyFrame->LoadData();
            keyFrames_.push_back(std::move(newKeyFrame));
        }
    } else {
        for (auto& keyFrame : keyFrames_) {
            keyFrame->LoadData();
        }
    }
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::SaveKeyFrames() {
    for (auto& keyFrame : keyFrames_) {
        keyFrame->SaveData();
    }
}

template <typename TSequenceElement>
const TSequenceElement* BaseSequenceEffectData<TSequenceElement>::GetSelectedKeyFrame() const {
    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        return keyFrames_[selectedKeyFrameIndex_].get();
    }
    return nullptr;
}

template <typename TSequenceElement>
void BaseSequenceEffectData<TSequenceElement>::SetSelectedKeyFrameIndex(const int32_t& index) {
    if (index >= -1 && index < static_cast<int32_t>(keyFrames_.size())) {
        selectedKeyFrameIndex_ = index;
    }
}

// 明示的なインスタンス化
template class BaseSequenceEffectData<CameraKeyFrame>;
template class BaseSequenceEffectData<ObjEaseAnimationSection>;
template class BaseSequenceEffectData<RailControlPoint>;