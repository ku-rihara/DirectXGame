#include "ObjEaseAnimationData.h"

using namespace KetaEngine;
#include "3d/WorldTransform.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "MathFunction.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>

void ObjEaseAnimationData::Init(const std::string& animationName, const std::string& categoryName) {
    BaseSequenceEffectData::Init(animationName, categoryName);

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
    UpdateActiveSection(speedRate);
    UpdateIndependentSRTProgression();
}

void ObjEaseAnimationData::UpdateActiveSection(float speedRate) {
    if (sectionElements_.empty()) {
        return;
    }

    // 各SRTが使用しているセクションを全て更新
    // どのセクションがアクティブかはSRTごとに異なる可能性があるため、
    // 使用中の全セクションを更新する
    std::array<bool, 16> updatedSections = {}; // 最大16セクションまで対応

    for (size_t srtType = 0; srtType < static_cast<size_t>(TransformType::Count); ++srtType) {
        int32_t sectionIndex = activeSectionIndices_[srtType];
        if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
            if (!updatedSections[sectionIndex]) {
                sectionElements_[sectionIndex]->Update(speedRate);
                updatedSections[sectionIndex] = true;
            }
        }
    }
}

void ObjEaseAnimationData::UpdateKeyFrameProgression() {
  
}

void ObjEaseAnimationData::UpdateIndependentSRTProgression() {
    if (sectionElements_.empty() || playState_ != PlayState::PLAYING) {
        return;
    }

    int32_t totalSections = static_cast<int32_t>(sectionElements_.size());

    // 各SRTを独立してチェック
    for (size_t srtType = 0; srtType < static_cast<size_t>(TransformType::Count); ++srtType) {
        TransformType type = static_cast<TransformType>(srtType);
        // ObjEaseAnimationSection用のTransformType
        auto sectionType = static_cast<ObjEaseAnimationSection::TransformType>(srtType);

        // 既に全セクション完了している場合はスキップ
        if (srtAllSectionsFinished_[srtType]) {
            continue;
        }

        int32_t currentSectionIndex = activeSectionIndices_[srtType];

        // 現在のセクションでこのSRTが完了したかチェック
        if (currentSectionIndex >= 0 && currentSectionIndex < totalSections) {
            if (sectionElements_[currentSectionIndex]->IsTransformFinished(sectionType)) {
                // 最後のセクションかチェック
                if (currentSectionIndex == totalSections - 1) {
                    srtAllSectionsFinished_[srtType] = true;
                } else {
                    // 次のセクションへ進む
                    AdvanceTransformToNextSection(type);
                }
            }
        }
    }

    // 全SRTが全セクション完了したかチェック
    if (AreAllSRTFinished()) {
        isAllKeyFramesFinished_ = true;
        playState_              = PlayState::STOPPED;
    }
}

void ObjEaseAnimationData::AdvanceTransformToNextSection(TransformType type) {
    size_t srtIndex          = static_cast<size_t>(type);
    int32_t currentSection   = activeSectionIndices_[srtIndex];
    int32_t nextSectionIndex = currentSection + 1;

    if (nextSectionIndex >= static_cast<int32_t>(sectionElements_.size())) {
        srtAllSectionsFinished_[srtIndex] = true;
        return;
    }

    // ObjEaseAnimationSection用のTransformType
    auto sectionType = static_cast<ObjEaseAnimationSection::TransformType>(srtIndex);

    // 現在のセクションからこのSRTの最終値を取得
    Vector3 currentValue;
    switch (type) {
    case TransformType::Scale:
        currentValue = sectionElements_[currentSection]->GetCurrentScale();
        break;
    case TransformType::Rotation:
        currentValue = sectionElements_[currentSection]->GetCurrentRotation();
        break;
    case TransformType::Translation:
        currentValue = sectionElements_[currentSection]->GetCurrentTranslation();
        break;
    default:
        return;
    }

    // 次のセクションへ進む
    activeSectionIndices_[srtIndex] = nextSectionIndex;

    // 次のセクションの開始値を設定して再生開始
    sectionElements_[nextSectionIndex]->SetStartValueForTransform(sectionType, currentValue);
    sectionElements_[nextSectionIndex]->StartPlayingForTransform(sectionType);
}

bool ObjEaseAnimationData::AreAllSRTFinished() const {
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        if (!srtAllSectionsFinished_[i]) {
            return false;
        }
    }
    return true;
}

void ObjEaseAnimationData::AdvanceToNextSequenceElement() {
  
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

    // SRT独立進行用のリセット
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        activeSectionIndices_[i]     = 0;
        srtAllSectionsFinished_[i]   = false;
    }
}

void ObjEaseAnimationData::Play() {
    BaseEffectData::Play();

    Vector3 scale     = originalValues_[static_cast<size_t>(TransformType::Scale)];
    Vector3 rotate    = originalValues_[static_cast<size_t>(TransformType::Rotation)];
    Vector3 transform = originalValues_[static_cast<size_t>(TransformType::Translation)];

    // SRT独立進行用のリセット
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        activeSectionIndices_[i]   = 0;
        srtAllSectionsFinished_[i] = false;
    }

    // 全てのキーフレームをリセット
    for (auto& section : sectionElements_) {
        section->StartWaiting();
        section->SetStartValues(scale, rotate, transform);
    }
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

    // SRT独立進行用の初期化
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        activeSectionIndices_[i]   = 0;
        srtAllSectionsFinished_[i] = false;
    }
}

std::unique_ptr<ObjEaseAnimationSection> ObjEaseAnimationData::CreateKeyFrame(int32_t index) {
    auto keyFrame = std::make_unique<ObjEaseAnimationSection>();
    keyFrame->Init(groupName_, categoryName_, index);
    return keyFrame;
}

std::string ObjEaseAnimationData::GetSequenceElementFolderPath() const {
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
    // TranslationのセクションインデックスでRailPlayerを取得
    int32_t sectionIndex = activeSectionIndices_[static_cast<size_t>(TransformType::Translation)];
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
        auto* railPlayer = sectionElements_[sectionIndex]->GetRailPlayer();
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

    // 各SRTは独立したセクションインデックスを持つ
    int32_t sectionIndex = activeSectionIndices_[static_cast<size_t>(type)];

    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
        switch (type) {
        case TransformType::Scale:
            return sectionElements_[sectionIndex]->GetCurrentScale();
        case TransformType::Rotation:
            return sectionElements_[sectionIndex]->GetCurrentRotation();
        case TransformType::Translation:
            return sectionElements_[sectionIndex]->GetCurrentTranslation();
        default:
            break;
        }
    }

    return Vector3::ZeroVector();
}

bool ObjEaseAnimationData::GetIsUseRailActiveKeyFrame() const {
    // TranslationのセクションインデックスでRail使用状態をチェック
    int32_t sectionIndex = activeSectionIndices_[static_cast<size_t>(TransformType::Translation)];
    bool isUseRail       = false;
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
        isUseRail = sectionElements_[sectionIndex]->IsUsingRail();
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

  
    if (ImGui::Button("Play this Date")) {
        Play();
    }

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

void ObjEaseAnimationData::SetPreAnimationOffsets(const Vector3& scale, const Vector3& rotation, const Vector3& translation) {
    for (auto& section : sectionElements_) {
        section->SetPreAnimationOffsets(scale, rotation, translation);
    }
}

