#include "CameraKeyFrame.h"
#include"MathFunction.h"
#include "Frame/Frame.h" 
#include <imgui.h>
#include <iostream>

void CameraKeyFrame::Init(const std::string& cameraAnimationName, const int32_t& keyNumber) {
    // グローバルパラメータ
    globalParameter_         = GlobalParameter::GetInstance();
    currentKeyFrameIndex     = keyNumber;
    std::string newGroupName = cameraAnimationName + std::to_string(currentKeyFrameIndex);

    groupName_ = newGroupName;
    globalParameter_->CreateGroup(groupName_, false);

   
    BindParams();

    AdaptValueSetting();
    AdaptEaseParam();
}

void CameraKeyFrame::Reset() {
    positionEase_.Reset();
    rotationEase_.Reset();
    fovEase_.Reset();
    AdaptValueSetting();
    AdaptEaseParam();
}

void CameraKeyFrame::LoadData() {

    // パラメータファイルから読み込み
    globalParameter_->LoadFile(groupName_, folderName_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void CameraKeyFrame::SaveData() {
    // パラメータファイルに保存
    globalParameter_->SaveFile(groupName_, folderName_);
}

void CameraKeyFrame::Update(const float& speedRate) {
    float actualDeltaTime;
    switch (static_cast<TimeMode>(timeMode_)) {
    case TimeMode::DELTA_TIME:
        // タイムスケール無視
        actualDeltaTime = Frame::DeltaTime() * speedRate; 
        break;
    case TimeMode::DELTA_TIME_RATE:
    default:
        // タイムスケール適用
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    }

    positionEase_.Update(actualDeltaTime);
    rotationEase_.Update(actualDeltaTime);
    fovEase_.Update(actualDeltaTime);
}

void CameraKeyFrame::BindParams() {
    globalParameter_->Bind(groupName_, "timePoint", &timePoint_);
    globalParameter_->Bind(groupName_, "position", &keyFrameParam_.position);
    globalParameter_->Bind(groupName_, "rotation", &keyFrameParam_.rotation);
    globalParameter_->Bind(groupName_, "fov", &keyFrameParam_.fov);
    globalParameter_->Bind(groupName_, "positionEaseType", &positionEaseType_);
    globalParameter_->Bind(groupName_, "rotationEaseType", &rotationEaseType_);
    globalParameter_->Bind(groupName_, "fovEaseType", &fovEaseType_);
    globalParameter_->Bind(groupName_, "timeMode", &timeMode_);
}

void CameraKeyFrame::AdjustParam() {
#ifdef _DEBUG

    ImGui::SeparatorText(("Camera KeyFrame: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    ImGui::DragFloat("Time Point", &timePoint_, 0.01f);

    ImGui::DragFloat3("Position", &keyFrameParam_.position.x, 0.1f);

    Vector3 rotationDegrees = ToDegree(keyFrameParam_.rotation);

    if (ImGui::DragFloat3("Rotation (Degrees)", &rotationDegrees.x, 1.0f)) {
   
        keyFrameParam_.rotation = ToRadian(rotationDegrees);
    }

    ImGui::DragFloat("FOV", &keyFrameParam_.fov, 0.01f);

    ImGui::Separator();

    // タイムモード設定
    TimeModeSelector("Time Mode", timeMode_);

    ImGui::Separator();

    // イージングタイプの設定
    EasingTypeSelector("Easing Type Position", positionEaseType_);
    EasingTypeSelector("Easing Type Rotate", rotationEaseType_);
    EasingTypeSelector("Easing Type Fov", fovEaseType_);

    AdaptEaseParam();

    ImGui::PopID();

#endif // _DEBUG
}

void CameraKeyFrame::AdaptEaseParam() {
    // parameterをEasingに適応
    positionEase_.SetMaxTime(timePoint_);
    rotationEase_.SetMaxTime(timePoint_);
    fovEase_.SetMaxTime(timePoint_);

    positionEase_.SetEndValue(keyFrameParam_.position);
    rotationEase_.SetEndValue(keyFrameParam_.rotation);
    fovEase_.SetEndValue(keyFrameParam_.fov);

    positionEase_.SetType(static_cast<EasingType>(positionEaseType_));
    rotationEase_.SetType(static_cast<EasingType>(rotationEaseType_));
    fovEase_.SetType(static_cast<EasingType>(fovEaseType_));
}

void CameraKeyFrame::AdaptValueSetting() {
    // adapt
    positionEase_.SetAdaptValue(&currentKeyFrameParam_.position);
    rotationEase_.SetAdaptValue(&currentKeyFrameParam_.rotation);
    fovEase_.SetAdaptValue(&currentKeyFrameParam_.fov);
}

void CameraKeyFrame::EasingTypeSelector(const char* label, int32_t& target) {
    int type = static_cast<int>(target);
    if (ImGui::Combo(label, &type, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
        target = type;
    }
}

void CameraKeyFrame::TimeModeSelector(const char* label, int32_t& target) {
    int mode = static_cast<int>(target);
    if (ImGui::Combo(label, &mode, TimeModeLabels.data(), static_cast<int>(TimeModeLabels.size()))) {
        target = mode;
    }
}

bool CameraKeyFrame::IsFinished() const {

    return positionEase_.IsFinished() && rotationEase_.IsFinished() && fovEase_.IsFinished();
}

void CameraKeyFrame::SetStartEasing(const Vector3& pos, const Vector3& rotate, const float& fov) {
    positionEase_.SetStartValue(pos);
    rotationEase_.SetStartValue(rotate);
    fovEase_.SetStartValue(fov);
}