#include "ObjEaseAnimationData.h"
#include "3d/WorldTransform.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "MathFunction.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>

void ObjEaseAnimationData::InitWithCategory(const std::string& animationName, const std::string& categoryName) {
    BaseSequenceEffectData::InitWithCategory(animationName, categoryName);

    groupName_  = animationName;
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

void ObjEaseAnimationData::Update(float speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }
    UpdateKeyFrameProgression();
    UpdateActiveSection(speedRate);
}

void ObjEaseAnimationData::UpdateActiveSection(float speedRate) {
    if (sectionElements_.empty()) {
        return;
    }

    // 現在のアクティブキーフレームを更新
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        sectionElements_[activeKeyFrameIndex_]->Update(speedRate);
    }
}

void ObjEaseAnimationData::UpdateKeyFrameProgression() {
    if (sectionElements_.empty() || playState_ != PlayState::PLAYING) {
        return;
    }

    // 現在のキーフレームが完了したかチェック
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        if (!sectionElements_[activeKeyFrameIndex_]->IsFinished()) {
            return; // まだ完了していない
        }

        // 最後のキーフレームかチェック
        if (activeKeyFrameIndex_ == static_cast<int32_t>(sectionElements_.size()) - 1) {
            isAllKeyFramesFinished_ = true;
            playState_              = PlayState::STOPPED;
        } else {
            AdvanceToNexTSequenceElement();
        }
    }
}

void ObjEaseAnimationData::AdvanceToNexTSequenceElement() {
    if (activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size()) - 1) {
        activeKeyFrameIndex_++;

        if (activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
            // 前のキーフレームの最終値を次のキーフレームの開始値として設定
            Vector3 startScale       = GetActiveKeyFrameValue(TransformType::Scale);
            Vector3 startRotation    = GetActiveKeyFrameValue(TransformType::Rotation);
            Vector3 startTranslation = GetActiveKeyFrameValue(TransformType::Translation);

            sectionElements_[activeKeyFrameIndex_]->SetStartValues(startScale, startRotation, startTranslation);
        }
    }
}

void ObjEaseAnimationData::Reset() {
    // 全てのキーフレームをリセット
    for (auto& section : sectionElements_) {
        section->Reset();
    }

    // 状態をリセット
    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
    playState_              = PlayState::STOPPED;
}

void ObjEaseAnimationData::Play() {
    BaseEffectData::Play();

    Vector3 scale     = originalValues_[static_cast<size_t>(TransformType::Scale)];
    Vector3 rotate    = originalValues_[static_cast<size_t>(TransformType::Rotation)];
    Vector3 transform = originalValues_[static_cast<size_t>(TransformType::Translation)];

    // 全てのキーフレームをリセット
    for (auto& section : sectionElements_) {
        section->StartWaiting();
        section->SetStartValues(scale, rotate, transform);
    }
}

void ObjEaseAnimationData::SetOriginalValues(const Vector3& scale, const Vector3& rotation, const Vector3& translation) {
    originalValues_[static_cast<size_t>(TransformType::Scale)]       = scale;
    originalValues_[static_cast<size_t>(TransformType::Rotation)]    = rotation;
    originalValues_[static_cast<size_t>(TransformType::Translation)] = translation;
}

void ObjEaseAnimationData::RegisterParams() {
    // originalValues_をパラメータとして登録
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        std::string srtName = GetSRTName(static_cast<TransformType>(i));
        globalParameter_->Regist(groupName_, "Original_" + srtName, &originalValues_[i]);
    }
}

void ObjEaseAnimationData::GetParams() {
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        std::string srtName = GetSRTName(static_cast<TransformType>(i));
        originalValues_[i]  = globalParameter_->GetValue<Vector3>(groupName_, "Original_" + srtName);
    }
}

void ObjEaseAnimationData::InitParams() {
    playState_              = PlayState::STOPPED;
    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
}

std::unique_ptr<ObjEaseAnimationSection> ObjEaseAnimationData::CreateKeyFrame(int32_t index) {
    auto keyFrame = std::make_unique<ObjEaseAnimationSection>();
    keyFrame->Init(groupName_, categoryName_, index);
    return keyFrame;
}

std::string ObjEaseAnimationData::GeTSequenceElementFolderPath() const {
    return baseFolderPath_ + categoryName_ + "/" + "Sections/" + groupName_ + "/";
}

void ObjEaseAnimationData::CreateOrLoadSections(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles) {
    if (sectionElements_.size() == 0) {
        for (const auto& [index, fileName] : KeyFrameFiles) {
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

RailPlayer* ObjEaseAnimationData::GetCurrentRailPlayer() const {
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        auto* railPlayer = sectionElements_[activeKeyFrameIndex_]->GetRailPlayer();
        if (railPlayer) {
            return railPlayer;
        }
    }
    return nullptr;
}

Vector3 ObjEaseAnimationData::GetActiveKeyFrameValue(const TransformType& type) const {
    if (sectionElements_.empty()) {
        return originalValues_[static_cast<size_t>(type)];
    }

    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        switch (type) {
        case TransformType::Scale:
            return sectionElements_[activeKeyFrameIndex_]->GetCurrentScale();
        case TransformType::Rotation:
            return sectionElements_[activeKeyFrameIndex_]->GetCurrentRotation();
        case TransformType::Translation:
            return sectionElements_[activeKeyFrameIndex_]->GetCurrentTranslation();
        default:
            break;
        }
    }

    return Vector3::ZeroVector();
}

bool ObjEaseAnimationData::GetIsUseRailActiveKeyFrame() const {
    bool isUseRail = false;
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        isUseRail = sectionElements_[activeKeyFrameIndex_]->IsUsingRail();
    }
    return isUseRail;
}

void ObjEaseAnimationData::AdjustParam() {
#ifdef _DEBUG

    ImGui::Text("Category: %s", categoryName_.c_str());
    ImGui::Text("Animation: %s", groupName_.c_str());

    // OriginTransform 編集
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        std::string srtName = GetSRTName(static_cast<TransformType>(i));
        ImGui::DragFloat3(("Origin" + srtName).c_str(), &originalValues_[i].x, 0.01f);
    }

    // 現在の値表示
    ImGui::SeparatorText("Current Animation Values");
    Vector3 currentScale = GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Scale);
    Vector3 currentRot   = GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Rotation);
    Vector3 currentTrans = GetActiveKeyFrameValue(ObjEaseAnimationData::TransformType::Translation);

    ImGui::Text("Scale: (%.2f, %.2f, %.2f)", currentScale.x, currentScale.y, currentScale.z);
    ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", currentRot.x, currentRot.y, currentRot.z);
    ImGui::Text("Translation: (%.2f, %.2f, %.2f)", currentTrans.x, currentTrans.y, currentTrans.z);

    // KeyFrame Controls
    ImGui::Separator();
    ImGui::Text("Sections: %d", GetTotalKeyFrameCount());

    if (ImGui::Button("Add Section")) {
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
        if (ImGui::Selectable(("Section " + std::to_string(i)).c_str(), isSelected)) {
            SetSelectedKeyFrameIndex(i);
        }
        ImGui::PopID();
    }

    // Selected KeyFrame Edit
    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < GetTotalKeyFrameCount()) {
        ImGui::Separator();
        sectionElements_[selectedKeyFrameIndex_]->AdjustParam();
    }

#endif
}

std::string ObjEaseAnimationData::GetSRTName(const TransformType& type) const {
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

void ObjEaseAnimationData::LoadSequenceElements() {
    BaseSequenceEffectData::LoadSequenceElements();
}

void ObjEaseAnimationData::SaveSequenceElements() {
    BaseSequenceEffectData::SaveSequenceElements();
}