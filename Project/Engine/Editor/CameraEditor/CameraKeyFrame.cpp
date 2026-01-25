#include "CameraKeyFrame.h"

using namespace KetaEngine;
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <imgui.h>
#include <iostream>

void CameraKeyFrame::Init(const std::string& groupName, const std::string& categoryName, int32_t keyNumber) {
    // グローバルパラメータ
    globalParameter_            = GlobalParameter::GetInstance();
    currenTSequenceElementIndex = keyNumber;
    categoryName_               = categoryName;
    std::string newGroupName    = groupName + std::to_string(currenTSequenceElementIndex);
    groupName_                  = newGroupName;

    folderPath_ = "CameraAnimation/" + categoryName_ + "/KeyFrames/" + groupName;

    if (!globalParameter_->HasRegisters(groupName_)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();

    } else {
        // パラメータを取得
        GetParams();
    }

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
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void CameraKeyFrame::SaveData() {
    // パラメータファイルに保存
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void CameraKeyFrame::Update(float speedRate) {
    float actualDeltaTime;
    switch (static_cast<TimeMode>(timeModeSelector_.GetTimeModeInt())) {
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

void CameraKeyFrame::RegisterParams() {
    globalParameter_->Regist(groupName_, "timePoint", &timePoint_);
    globalParameter_->Regist(groupName_, "position", &keyFrameParam_.position);
    globalParameter_->Regist(groupName_, "rotation", &keyFrameParam_.rotation);
    globalParameter_->Regist(groupName_, "fov", &keyFrameParam_.fov);
    globalParameter_->Regist(groupName_, "positionEaseType", &positionEaseType_);
    globalParameter_->Regist(groupName_, "rotationEaseType", &rotationEaseType_);
    globalParameter_->Regist(groupName_, "fovEaseType", &fovEaseType_);
    timeModeSelector_.RegisterParam(groupName_, globalParameter_);
}

void CameraKeyFrame::GetParams() {
    // timePoint
    timePoint_ = globalParameter_->GetValue<float>(groupName_, "timePoint");

    // KeyFrameParam
    keyFrameParam_.position = globalParameter_->GetValue<Vector3>(groupName_, "position");
    keyFrameParam_.rotation = globalParameter_->GetValue<Vector3>(groupName_, "rotation");
    keyFrameParam_.fov      = globalParameter_->GetValue<float>(groupName_, "fov");

    // EaseType
    positionEaseType_ = globalParameter_->GetValue<int32_t>(groupName_, "positionEaseType");
    rotationEaseType_ = globalParameter_->GetValue<int32_t>(groupName_, "rotationEaseType");
    fovEaseType_      = globalParameter_->GetValue<int32_t>(groupName_, "fovEaseType");

    // TimeMode
    timeModeSelector_.GetParam(groupName_, globalParameter_);
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
    timeModeSelector_.SelectTimeModeImGui("Time Mode");

    ImGui::Separator();

    // イージングタイプの設定
    ImGuiEasingTypeSelector("Easing Type Position", positionEaseType_);
    ImGuiEasingTypeSelector("Easing Type Rotate", rotationEaseType_);
    ImGuiEasingTypeSelector("Easing Type Fov", fovEaseType_);

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
    positionEase_.SetAdaptValue(&currenTSequenceElementParam_.position);
    rotationEase_.SetAdaptValue(&currenTSequenceElementParam_.rotation);
    fovEase_.SetAdaptValue(&currenTSequenceElementParam_.fov);
}

bool CameraKeyFrame::IsFinished() const {
    return positionEase_.IsFinished() && rotationEase_.IsFinished() && fovEase_.IsFinished();
}

void CameraKeyFrame::SetStartEasing(const Vector3& pos, const Vector3& rotate, float fov) {
    positionEase_.SetStartValue(pos);
    rotationEase_.SetStartValue(rotate);
    fovEase_.SetStartValue(fov);
}