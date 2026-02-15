#include "SpriteEaseAnimationData.h"

using namespace KetaEngine;
#include "MathFunction.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>

void SpriteEaseAnimationData::Init(const std::string& animationName, const std::string& categoryName) {
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

void SpriteEaseAnimationData::Update(float speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }
    UpdateActiveSection(speedRate);
    UpdateIndependentProgression();
}

void SpriteEaseAnimationData::UpdateActiveSection(float speedRate) {
    if (sectionElements_.empty()) {
        return;
    }

    // 各プロパティが使用しているセクションを全て更新
    std::array<bool, 16> updatedSections = {};

    for (size_t propType = 0; propType < kPropertyCount; ++propType) {
        int32_t sectionIndex = activeSectionIndices_[propType];
        if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
            if (!updatedSections[sectionIndex]) {
                sectionElements_[sectionIndex]->Update(speedRate);
                updatedSections[sectionIndex] = true;
            }
        }
    }
}

void SpriteEaseAnimationData::UpdateKeyFrameProgression() {
}

void SpriteEaseAnimationData::UpdateIndependentProgression() {
    if (sectionElements_.empty() || playState_ != PlayState::PLAYING) {
        return;
    }

    int32_t totalSections = static_cast<int32_t>(sectionElements_.size());

    for (size_t propIdx = 0; propIdx < kPropertyCount; ++propIdx) {
        PropertyType type = static_cast<PropertyType>(propIdx);
        auto sectionType  = static_cast<SpriteEaseAnimationSection::PropertyType>(propIdx);

        if (allSectionsFinished_[propIdx]) {
            continue;
        }

        int32_t currentSectionIndex = activeSectionIndices_[propIdx];

        if (currentSectionIndex >= 0 && currentSectionIndex < totalSections) {
            if (sectionElements_[currentSectionIndex]->IsPropertyFinished(sectionType)) {
                if (currentSectionIndex == totalSections - 1) {
                    allSectionsFinished_[propIdx] = true;
                } else {
                    AdvancePropertyToNextSection(type);
                }
            }
        }
    }

    if (AreAllPropertiesFinished()) {
        isAllKeyFramesFinished_ = true;
        playState_              = PlayState::STOPPED;
    }
}

void SpriteEaseAnimationData::AdvancePropertyToNextSection(PropertyType type) {
    size_t propIndex         = static_cast<size_t>(type);
    int32_t currentSection   = activeSectionIndices_[propIndex];
    int32_t nextSectionIndex = currentSection + 1;

    if (nextSectionIndex >= static_cast<int32_t>(sectionElements_.size())) {
        allSectionsFinished_[propIndex] = true;
        return;
    }

    auto sectionType = static_cast<SpriteEaseAnimationSection::PropertyType>(propIndex);

    // 現在のセクションからこのプロパティの最終値を取得し、次のセクションの開始値に設定
    activeSectionIndices_[propIndex] = nextSectionIndex;

    switch (type) {
    case PropertyType::Scale: {
        Vector2 currentValue = sectionElements_[currentSection]->GetCurrentScale();
        sectionElements_[nextSectionIndex]->SetStartValueForProperty_Vec2(sectionType, currentValue);
        break;
    }
    case PropertyType::Position: {
        Vector2 currentValue = sectionElements_[currentSection]->GetCurrentPosition();
        sectionElements_[nextSectionIndex]->SetStartValueForProperty_Vec2(sectionType, currentValue);
        break;
    }
    case PropertyType::Rotation: {
        Vector3 currentValue = sectionElements_[currentSection]->GetCurrentRotation();
        sectionElements_[nextSectionIndex]->SetStartValueForProperty_Vec3(sectionType, currentValue);
        break;
    }
    case PropertyType::Color: {
        Vector3 currentValue = sectionElements_[currentSection]->GetCurrentColor();
        sectionElements_[nextSectionIndex]->SetStartValueForProperty_Vec3(sectionType, currentValue);
        break;
    }
    case PropertyType::Alpha: {
        float currentValue = sectionElements_[currentSection]->GetCurrentAlpha();
        sectionElements_[nextSectionIndex]->SetStartValueForProperty_Float(sectionType, currentValue);
        break;
    }
    default:
        return;
    }

    sectionElements_[nextSectionIndex]->StartPlayingForProperty(sectionType);
}

bool SpriteEaseAnimationData::AreAllPropertiesFinished() const {
    for (size_t i = 0; i < kPropertyCount; ++i) {
        if (!allSectionsFinished_[i]) {
            return false;
        }
    }
    return true;
}

void SpriteEaseAnimationData::AdvanceToNextSequenceElement() {
}

void SpriteEaseAnimationData::Reset() {
    for (auto& section : sectionElements_) {
        section->Reset();
    }

    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;
    playState_              = PlayState::STOPPED;

    for (size_t i = 0; i < kPropertyCount; ++i) {
        activeSectionIndices_[i]  = 0;
        allSectionsFinished_[i]   = false;
    }
}

void SpriteEaseAnimationData::Play() {
    BaseEffectData::Play();

    for (size_t i = 0; i < kPropertyCount; ++i) {
        activeSectionIndices_[i] = 0;
        allSectionsFinished_[i]  = false;
    }

    for (auto& section : sectionElements_) {
        section->StartWaiting();
        section->SetStartValues(originalScale_, originalPosition_, originalRotation_,
                                originalColor_, originalAlpha_);
    }
}

void SpriteEaseAnimationData::RegisterParams() {
    globalParameter_->Regist(groupName_, "Original_Scale", &originalScale_);
    globalParameter_->Regist(groupName_, "Original_Position", &originalPosition_);
    globalParameter_->Regist(groupName_, "Original_Rotation", &originalRotation_);
    globalParameter_->Regist(groupName_, "Original_Color", &originalColor_);
    globalParameter_->Regist(groupName_, "Original_Alpha", &originalAlpha_);
}

void SpriteEaseAnimationData::GetParams() {
    originalScale_    = globalParameter_->GetValue<Vector2>(groupName_, "Original_Scale");
    originalPosition_ = globalParameter_->GetValue<Vector2>(groupName_, "Original_Position");
    originalRotation_ = globalParameter_->GetValue<Vector3>(groupName_, "Original_Rotation");
    originalColor_    = globalParameter_->GetValue<Vector3>(groupName_, "Original_Color");
    originalAlpha_    = globalParameter_->GetValue<float>(groupName_, "Original_Alpha");
}

void SpriteEaseAnimationData::InitParams() {
    playState_              = PlayState::STOPPED;
    activeKeyFrameIndex_    = 0;
    isAllKeyFramesFinished_ = false;

    for (size_t i = 0; i < kPropertyCount; ++i) {
        activeSectionIndices_[i] = 0;
        allSectionsFinished_[i]  = false;
    }
}

std::unique_ptr<SpriteEaseAnimationSection> SpriteEaseAnimationData::CreateKeyFrame(int32_t index) {
    auto keyFrame = std::make_unique<SpriteEaseAnimationSection>();
    keyFrame->Init(groupName_, categoryName_, index);
    return keyFrame;
}

std::string SpriteEaseAnimationData::GetSequenceElementFolderPath() const {
    return baseFolderPath_ + categoryName_ + "/" + "Sections/" + groupName_ + "/";
}

Vector2 SpriteEaseAnimationData::GetActiveScale() const {
    if (sectionElements_.empty()) {
        return originalScale_;
    }
    int32_t sectionIndex = activeSectionIndices_[static_cast<size_t>(PropertyType::Scale)];
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
        return sectionElements_[sectionIndex]->GetCurrentScale();
    }
    return originalScale_;
}

Vector2 SpriteEaseAnimationData::GetActivePosition() const {
    if (sectionElements_.empty()) {
        return originalPosition_;
    }
    int32_t sectionIndex = activeSectionIndices_[static_cast<size_t>(PropertyType::Position)];
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
        return sectionElements_[sectionIndex]->GetCurrentPosition();
    }
    return originalPosition_;
}

Vector3 SpriteEaseAnimationData::GetActiveRotation() const {
    if (sectionElements_.empty()) {
        return originalRotation_;
    }
    int32_t sectionIndex = activeSectionIndices_[static_cast<size_t>(PropertyType::Rotation)];
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
        return sectionElements_[sectionIndex]->GetCurrentRotation();
    }
    return originalRotation_;
}

Vector3 SpriteEaseAnimationData::GetActiveColor() const {
    if (sectionElements_.empty()) {
        return originalColor_;
    }
    int32_t sectionIndex = activeSectionIndices_[static_cast<size_t>(PropertyType::Color)];
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
        return sectionElements_[sectionIndex]->GetCurrentColor();
    }
    return originalColor_;
}

float SpriteEaseAnimationData::GetActiveAlpha() const {
    if (sectionElements_.empty()) {
        return originalAlpha_;
    }
    int32_t sectionIndex = activeSectionIndices_[static_cast<size_t>(PropertyType::Alpha)];
    if (sectionIndex >= 0 && sectionIndex < static_cast<int32_t>(sectionElements_.size())) {
        return sectionElements_[sectionIndex]->GetCurrentAlpha();
    }
    return originalAlpha_;
}

void SpriteEaseAnimationData::AdjustParam() {
#ifdef _DEBUG

    ImGui::Text("Category: %s", categoryName_.c_str());
    ImGui::Text("Animation: %s", groupName_.c_str());

    // OriginValues 編集
    ImGui::DragFloat2("OriginScale", &originalScale_.x, 0.01f);
    ImGui::DragFloat2("OriginPosition", &originalPosition_.x, 0.1f);
    ImGui::DragFloat3("OriginRotation", &originalRotation_.x, 0.01f);
    ImGui::ColorEdit3("OriginColor", &originalColor_.x);
    ImGui::SliderFloat("OriginAlpha", &originalAlpha_, 0.0f, 1.0f);

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

const char* SpriteEaseAnimationData::GetPropertyName(PropertyType type) const {
    switch (type) {
    case PropertyType::Scale:    return "Scale";
    case PropertyType::Position: return "Position";
    case PropertyType::Rotation: return "Rotation";
    case PropertyType::Color:    return "Color";
    case PropertyType::Alpha:    return "Alpha";
    default:                     return "Unknown";
    }
}

void SpriteEaseAnimationData::SetPreAnimationOffsets(const Vector2& scale, const Vector2& position,
                                                      const Vector3& rotation, const Vector3& color, float alpha) {
    for (auto& section : sectionElements_) {
        section->SetPreAnimationOffsets(scale, position, rotation, color, alpha);
    }
}
