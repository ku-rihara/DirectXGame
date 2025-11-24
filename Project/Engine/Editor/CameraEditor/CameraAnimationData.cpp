#include "CameraAnimationData.h"
#include "Frame/Frame.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>

void CameraAnimationData::Init(const std::string& animationName) {
    BaseSequenceEffectData::Init(animationName);

    folderPath_ = dateFolderPath_;

    if (!globalParameter_->HasRegisters(animationName)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        GetParams();
    }

    InitParams();
}

void CameraAnimationData::InitParams() {
    activeKeyFrameIndex_            = 0;
    isAllKeyFramesFinished_         = false;
    returnParam_.isWaitingForReturn = false;
    resetParam_.currentDelayTimer   = 0.0f;

    returnParam_.positionEase.SetAdaptValue(&returnCameraTransform_.position);
    returnParam_.rotationEase.SetAdaptValue(&returnCameraTransform_.rotation);
    returnParam_.fovEase.SetAdaptValue(&returnCameraTransform_.fov);
}

void CameraAnimationData::RegisterParams() {
    globalParameter_->Regist(groupName_, "autoReturnToInitial", &returnParam_.autoReturnToInitial);
    globalParameter_->Regist(groupName_, "resetPosEaseType", &resetParam_.posEaseType);
    globalParameter_->Regist(groupName_, "resetRotateEaseType", &resetParam_.rotateEaseType);
    globalParameter_->Regist(groupName_, "resetFovEaseType", &resetParam_.fovEaseType);
    globalParameter_->Regist(groupName_, "resetTimePoint", &resetParam_.timePoint);
    globalParameter_->Regist(groupName_, "returnDelayTime", &resetParam_.delayTime);
    globalParameter_->Regist(groupName_, "timeMode", &timeMode_);
}

void CameraAnimationData::GetParams() {
    returnParam_.autoReturnToInitial = globalParameter_->GetValue<bool>(groupName_, "autoReturnToInitial");
    resetParam_.posEaseType          = globalParameter_->GetValue<int32_t>(groupName_, "resetPosEaseType");
    resetParam_.rotateEaseType       = globalParameter_->GetValue<int32_t>(groupName_, "resetRotateEaseType");
    resetParam_.fovEaseType          = globalParameter_->GetValue<int32_t>(groupName_, "resetFovEaseType");
    resetParam_.timePoint            = globalParameter_->GetValue<float>(groupName_, "resetTimePoint");
    resetParam_.delayTime            = globalParameter_->GetValue<float>(groupName_, "returnDelayTime");
    timeMode_                        = globalParameter_->GetValue<int32_t>(groupName_, "timeMode");
}

void CameraAnimationData::Update(const float& speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    UpdateKeyFrameProgression();
    UpdateActiveKeyFrames(speedRate);
    UpdateInterpolatedValues();
}

void CameraAnimationData::UpdateActiveKeyFrames(const float& speedRate) {
    if (sectionElements_.empty()) {
        return;
    }

    float actualDeltaTime = 0.0f;
    switch (static_cast<CameraKeyFrame::TimeMode>(timeMode_)) {
    case CameraKeyFrame::TimeMode::DELTA_TIME:
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case CameraKeyFrame::TimeMode::DELTA_TIME_RATE:
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    default:
        break;
    }

    // returnEasing開始待機中の処理
    if (returnParam_.isWaitingForReturn) {
        resetParam_.currentDelayTimer += actualDeltaTime;

        if (resetParam_.currentDelayTimer >= resetParam_.delayTime) {
            returnParam_.isWaitingForReturn   = false;
            returnParam_.isReturningToInitial = true;
            resetParam_.currentDelayTimer     = 0.0f;
        }
        return;
    }

    // 初期値に戻るイージング
    if (returnParam_.isReturningToInitial) {
        returnParam_.positionEase.Update(actualDeltaTime);
        returnParam_.rotationEase.Update(actualDeltaTime);
        returnParam_.fovEase.Update(actualDeltaTime);

        if (returnParam_.positionEase.IsFinished() && returnParam_.rotationEase.IsFinished() && returnParam_.fovEase.IsFinished()) {
            returnParam_.isReturningToInitial = false;
            isAllFinished_                    = true;
            playState_                        = PlayState::STOPPED;
        }
        return;
    }

    // 現在のアクティブキーフレームを更新
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        sectionElements_[activeKeyFrameIndex_]->Update(speedRate);
    }
}

void CameraAnimationData::UpdateKeyFrameProgression() {
    if (sectionElements_.empty() || playState_ != PlayState::PLAYING) {
        return;
    }

    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        if (!sectionElements_[activeKeyFrameIndex_]->IsFinished()) {
            return;
        }

        // 最後のキーフレームかチェック
        if (activeKeyFrameIndex_ == static_cast<int32_t>(sectionElements_.size()) - 1) {
            isAllKeyFramesFinished_ = true;
            finalKeyFrameIndex_     = activeKeyFrameIndex_;

            if (returnParam_.autoReturnToInitial) {
                StartReturnToInitial();
            } else {
                playState_ = PlayState::STOPPED;
            }
        } else {
            AdvanceToNexTSequenceElement();
        }
    }
}

void CameraAnimationData::AdvanceToNexTSequenceElement() {
    if (activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size()) - 1) {
        activeKeyFrameIndex_++;

        if (activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
            Vector3 startPos = currentCameraTransform_.position;
            Vector3 startRot = currentCameraTransform_.rotation;
            float startFov   = currentCameraTransform_.fov;

            sectionElements_[activeKeyFrameIndex_]->SetStartEasing(startPos, startRot, startFov);
        }
    }
}

void CameraAnimationData::UpdateInterpolatedValues() {
    if (sectionElements_.empty()) {
        return;
    }

    if (returnParam_.isReturningToInitial) {
        currentCameraTransform_.position = returnCameraTransform_.position;
        currentCameraTransform_.rotation = returnCameraTransform_.rotation;
        currentCameraTransform_.fov      = returnCameraTransform_.fov;
    } else if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        currentCameraTransform_.position = sectionElements_[activeKeyFrameIndex_]->GetPosition();
        currentCameraTransform_.rotation = sectionElements_[activeKeyFrameIndex_]->GetRotation();
        currentCameraTransform_.fov      = sectionElements_[activeKeyFrameIndex_]->GetFov();
    }
}

void CameraAnimationData::ApplyToViewProjection(ViewProjection& viewProjection) {
    if (playState_ == PlayState::PLAYING) {
        viewProjection.positionOffset_ = currentCameraTransform_.position;
        viewProjection.rotationOffset_ = currentCameraTransform_.rotation;
        viewProjection.fovAngleY_      = currentCameraTransform_.fov;
    } else if (isAllFinished_) {
        viewProjection.positionOffset_ = initialCameraTransform_.position;
        viewProjection.rotationOffset_ = initialCameraTransform_.rotation;
        viewProjection.fovAngleY_      = initialCameraTransform_.fov;
    }
}

void CameraAnimationData::Reset() {
    for (auto& keyframe : sectionElements_) {
        keyframe->Reset();
    }

    returnParam_.positionEase.Reset();
    returnParam_.rotationEase.Reset();
    returnParam_.fovEase.Reset();

    if (!sectionElements_.empty() && activeKeyFrameIndex_ == 0) {
        sectionElements_[0]->SetStartEasing(
            initialCameraTransform_.position,
            initialCameraTransform_.rotation,
            initialCameraTransform_.fov);
    }

    isAllKeyFramesFinished_           = false;
    returnParam_.isReturningToInitial = false;
    returnParam_.isWaitingForReturn   = false;
    isAllFinished_                    = false;
    activeKeyFrameIndex_              = 0;
    resetParam_.currentDelayTimer     = 0.0f;
    playState_                        = PlayState::STOPPED;
}

void CameraAnimationData::StartReturnToInitial() {
    if (returnParam_.isReturningToInitial || returnParam_.isWaitingForReturn) {
        return;
    }

    if (resetParam_.delayTime > 0.0f) {
        returnParam_.isWaitingForReturn = true;
        resetParam_.currentDelayTimer   = 0.0f;
    } else {
        returnParam_.isReturningToInitial = true;
    }

    Vector3 currentPos    = currentCameraTransform_.position;
    Vector3 currentRot    = currentCameraTransform_.rotation;
    float currentFovValue = currentCameraTransform_.fov;

    returnParam_.positionEase.SetStartValue(currentPos);
    returnParam_.positionEase.SetEndValue(initialCameraTransform_.position);
    returnParam_.positionEase.SetMaxTime(resetParam_.timePoint);
    returnParam_.positionEase.SetType(static_cast<EasingType>(resetParam_.posEaseType));
    returnParam_.positionEase.Reset();

    returnParam_.rotationEase.SetStartValue(currentRot);
    returnParam_.rotationEase.SetEndValue(initialCameraTransform_.rotation);
    returnParam_.rotationEase.SetMaxTime(resetParam_.timePoint);
    returnParam_.rotationEase.SetType(static_cast<EasingType>(resetParam_.rotateEaseType));
    returnParam_.rotationEase.Reset();

    returnParam_.fovEase.SetStartValue(currentFovValue);
    returnParam_.fovEase.SetEndValue(initialCameraTransform_.fov);
    returnParam_.fovEase.SetMaxTime(resetParam_.timePoint);
    returnParam_.fovEase.SetType(static_cast<EasingType>(resetParam_.fovEaseType));
    returnParam_.fovEase.Reset();
}

void CameraAnimationData::SetInitialValues(const Vector3& position, const Vector3& rotation, const float& fov) {
    initialCameraTransform_.position = position;
    initialCameraTransform_.rotation = rotation;
    initialCameraTransform_.fov      = fov;
}

std::unique_ptr<CameraKeyFrame> CameraAnimationData::CreateKeyFrame(const int32_t& index) {
    auto keyFrame = std::make_unique<CameraKeyFrame>();
    keyFrame->Init(groupName_, index);
    return keyFrame;
}

std::string CameraAnimationData::GeTSequenceElementFolderPath() const {
    return keyFrameFolderPath_ + groupName_ + "/";
}

void CameraAnimationData::AdjustParam() {
#ifdef _DEBUG
    if (showAnimationControls_) {
        ImGui::SeparatorText(("Camera Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        ImGui::Checkbox("Auto Return to Initial", &returnParam_.autoReturnToInitial);

        if (isAllKeyFramesFinished_) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Animation Finished!");
        }

        if (returnParam_.isWaitingForReturn) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Waiting for Return (%.2f/%.2f)",
                resetParam_.currentDelayTimer, resetParam_.delayTime);
        }
        ImGui::PopID();
    }

    ImGui::SeparatorText("Reset Param");
    ImGui::DragFloat("Return Delay Time", &resetParam_.delayTime, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Reset Time Point", &resetParam_.timePoint, 0.01f);
    ImGuiEasingTypeSelector("Easing Type Position", resetParam_.posEaseType);
    ImGuiEasingTypeSelector("Easing Type Rotate", resetParam_.rotateEaseType);
    ImGuiEasingTypeSelector("Easing Type Fov", resetParam_.fovEaseType);

    ImGui::SeparatorText("deltaTime");
    TimeModeSelector("Time Mode", timeMode_);

    ImGui::SeparatorText("keyFrameEdit");
    if (showKeyFrameList_) {
        ImGui::Text("KeyFrames (%zu):", sectionElements_.size());
        for (int32_t i = 0; i < static_cast<int32_t>(sectionElements_.size()); ++i) {
            ImGui::PushID(i);

            bool isSelected       = (selectedKeyFrameIndex_ == i);
            bool isActive         = (activeKeyFrameIndex_ == i);
            std::string labelText = "KeyFrame " + std::to_string(i) + " (t:" + std::to_string(sectionElements_[i]->GetTimePoint()) + ")";

            if (isActive) {
                labelText += " [ACTIVE]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            }

            if (ImGui::Selectable(labelText.c_str(), isSelected, 0, ImVec2(0, 0))) {
                selectedKeyFrameIndex_ = i;
            }

            if (isActive) {
                ImGui::PopStyleColor();
            }

            ImGui::PopID();
            ImGui::Spacing();
        }

        if (ImGui::Button("Add KeyFrame")) {
            AddKeyFrame();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear All KeyFrames")) {
            ClearKeyFrames();
        }
    }

    ImGui::Separator();
    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < static_cast<int32_t>(sectionElements_.size())) {
        sectionElements_[selectedKeyFrameIndex_]->AdjustParam();
    }

    ImGui::SeparatorText("Current Values");
    ImGui::Text("Position: (%.2f, %.2f, %.2f)",
        currentCameraTransform_.position.x,
        currentCameraTransform_.position.y,
        currentCameraTransform_.position.z);
    ImGui::Text("Rotation: (%.2f, %.2f, %.2f)",
        currentCameraTransform_.rotation.x,
        currentCameraTransform_.rotation.y,
        currentCameraTransform_.rotation.z);
    ImGui::Text("FOV: %.2f", currentCameraTransform_.fov);
#endif
}

void CameraAnimationData::TimeModeSelector(const char* label, int32_t& target) {
    int mode = static_cast<int>(target);
    if (ImGui::Combo(label, &mode, TimeModeLabels.data(), static_cast<int>(TimeModeLabels.size()))) {
        target = mode;
    }
}

void CameraAnimationData::LoadSequenceElements() {
    BaseSequenceEffectData::LoadSequenceElements();
}
void CameraAnimationData::SaveSequenceElements() {
    BaseSequenceEffectData::SaveSequenceElements();
}
