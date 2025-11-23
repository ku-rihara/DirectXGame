#include "ObjEaseAnimationData.h"
#include "3d/WorldTransform.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "MathFunction.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>


void ObjEaseAnimationData::InitWithCategory(const std::string& animationName, const std::string& categoryName) {
    globalParameter_ = GlobalParameter::GetInstance();
    groupName_       = animationName;
    categoryName_    = categoryName;

    folderPath_ = baseFolderPath_ + categoryName_ + "/" + "Dates";

    if (!globalParameter_->HasRegisters(groupName_)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        LoadParams();
    }

    InitParams();
}

void ObjEaseAnimationData::Update(const float& speedRate) {
    (void)speedRate; // 未使用パラメータ警告を抑制
    UpdateKeyFrameProgression();
}

void ObjEaseAnimationData::UpdateKeyFrameProgression() {
    if (keyFrames_.empty() || playState_ != PlayState::PLAYING) {
        return;
    }

    // 現在のキーフレームを更新
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        keyFrames_[activeKeyFrameIndex_]->Update(1.0f);

        // 現在のキーフレームが完了したかチェック
        if (!keyFrames_[activeKeyFrameIndex_]->IsFinished()) {
            return;
        }

        // 最後のキーフレームかチェック
        if (activeKeyFrameIndex_ == static_cast<int32_t>(keyFrames_.size()) - 1) {
            isAllKeyFramesFinished_ = true;
            playState_              = PlayState::STOPPED;
        } else {
            AdvanceToNexTSequenceElement();
        }
    }
}

void ObjEaseAnimationData::AdvanceToNexTSequenceElement() {
    if (activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size()) - 1) {
        activeKeyFrameIndex_++;

        if (activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
            // 前のキーフレームの最終値を次のキーフレームの開始値として設定
            Vector3 startScale       = GetActiveKeyFrameValue(TransformType::Scale);
            Vector3 startRotation    = GetActiveKeyFrameValue(TransformType::Rotation);
            Vector3 startTranslation = GetActiveKeyFrameValue(TransformType::Translation);

            keyFrames_[activeKeyFrameIndex_]->SetStartValues(startScale, startRotation, startTranslation);
        }
    }
}

void ObjEaseAnimationData::Reset() {
    // 全てのキーフレームをリセット
    for (auto& keyFrame : keyFrames_) {
        keyFrame->Reset();
    }

    // 状態をリセット
    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
    playState_              = PlayState::STOPPED;
}

void ObjEaseAnimationData::RegisterParams() {
    // 元の値を登録
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const char* name = GetSRTName(static_cast<TransformType>(i));
        globalParameter_->Regist(groupName_, std::string(name) + "_OriginalValue", &originalValues_[i]);
    }
}

void ObjEaseAnimationData::LoadParams() {
    // 元の値を取得
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const char* name   = GetSRTName(static_cast<TransformType>(i));
        originalValues_[i] = globalParameter_->GetValue<Vector3>(groupName_, std::string(name) + "_OriginalValue");
    }
}

void ObjEaseAnimationData::InitParams() {
    playState_              = PlayState::STOPPED;
    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;

    // デフォルト値設定
    originalValues_[static_cast<size_t>(TransformType::Scale)]       = Vector3::OneVector();
    originalValues_[static_cast<size_t>(TransformType::Rotation)]    = Vector3::ZeroVector();
    originalValues_[static_cast<size_t>(TransformType::Translation)] = Vector3::ZeroVector();
}

std::unique_ptr<ObjEaseAnimationSection> ObjEaseAnimationData::CreateKeyFrame(const int32_t& index) {
    auto keyFrame = std::make_unique<ObjEaseAnimationSection>();
    keyFrame->Init(groupName_, categoryName_, index);
    return keyFrame;
}

std::string ObjEaseAnimationData::GeTSequenceElementFolderPath() const {
    return baseFolderPath_ + categoryName_ + "/" + "KeyFrames/" + groupName_ + "/";
}

void ObjEaseAnimationData::CreateOrLoadKeyFrames(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles) {
    if (keyFrames_.size() == 0) {
        ClearKeyFrames();
        for (const auto& [index, fileName] : KeyFrameFiles) {
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

const char* ObjEaseAnimationData::GetSRTName(const TransformType& type) const {
    switch (type) {
    case TransformType::Scale:
        return "Scale";
    case TransformType::Rotation:
        return "Rotation";
    case TransformType::Translation:
        return "Translation";
    default:
        return "Unknown";
    }
}

RailPlayer* ObjEaseAnimationData::GetCurrentRailPlayer() const {
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        auto* railPlayer = keyFrames_[activeKeyFrameIndex_]->GetRailPlayer();
        if (railPlayer) {
            return railPlayer;
        }
    }
    return nullptr;
}

Vector3 ObjEaseAnimationData::GetActiveKeyFrameValue(const TransformType& type) const {
    if (keyFrames_.empty()) {
        return originalValues_[static_cast<size_t>(type)];
    }

    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        switch (type) {
        case TransformType::Scale:
            return keyFrames_[activeKeyFrameIndex_]->GetCurrentScale();
        case TransformType::Rotation:
            return keyFrames_[activeKeyFrameIndex_]->GetCurrentRotation();
        case TransformType::Translation:
            return keyFrames_[activeKeyFrameIndex_]->GetCurrentTranslation();
        default:
            break;
        }
    }

    return Vector3::ZeroVector();
}

bool ObjEaseAnimationData::GetIsUseRailActiveKeyFrame() const {
    bool isUseRail = false;
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        isUseRail = keyFrames_[activeKeyFrameIndex_]->IsUsingRail();
    }
    return isUseRail;
}

void ObjEaseAnimationData::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::TreeNode("Animation Settings")) {
        ImGui::Text("Category: %s", categoryName_.c_str());
        ImGui::Text("Animation: %s", groupName_.c_str());

        // Original Values
        if (ImGui::TreeNode("Original Values")) {
            for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
                const char* name = GetSRTName(static_cast<TransformType>(i));
                ImGui::DragFloat3(name, &originalValues_[i].x, 0.01f);
            }
            ImGui::TreePop();
        }

        // KeyFrame Controls
        ImGui::Separator();
        ImGui::Text("KeyFrames: %d", GetTotalKeyFrameCount());

        if (ImGui::Button("Add KeyFrame")) {
            AddKeyFrame();
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove Selected") && selectedKeyFrameIndex_ >= 0) {
            RemoveKeyFrame(selectedKeyFrameIndex_);
        }

        // KeyFrame List
        for (int i = 0; i < GetTotalKeyFrameCount(); ++i) {
            ImGui::PushID(i);
            bool isSelected = (selectedKeyFrameIndex_ == i);
            if (ImGui::Selectable(("KeyFrame " + std::to_string(i)).c_str(), isSelected)) {
                SetSelectedKeyFrameIndex(i);
            }
            ImGui::PopID();
        }

        // Selected KeyFrame Edit
        if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < GetTotalKeyFrameCount()) {
            ImGui::Separator();
            keyFrames_[selectedKeyFrameIndex_]->AdjustParam();
        }

        ImGui::TreePop();
    }
#endif
}