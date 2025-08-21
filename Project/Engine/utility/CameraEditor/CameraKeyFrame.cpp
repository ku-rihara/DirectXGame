#include "CameraKeyFrame.h"
#include <imgui.h>
#include <iostream>

void CameraKeyFrame::Init(const std::string& cameraAnimationName, const int32_t& keyNumber) {
    // グローバルパラメータ
    globalParameter_         = GlobalParameter::GetInstance();
    currentKeyFrameIndex     = keyNumber;
    std::string newGroupName = cameraAnimationName + std::to_string(currentKeyFrameIndex);


    if (!groupName_.empty() && groupName_ != newGroupName) {
        globalParameter_->ClearBindingsForGroup(groupName_);
    }

    groupName_ = newGroupName;
    globalParameter_->CreateGroup(groupName_, false);

    // 重複バインドを防ぐ
    globalParameter_->ClearBindingsForGroup(groupName_);

    BindParams();

    // adapt
    positionEase_.SetAdaptValue(&currentKeyFrameParam_.position);
    rotationEase_.SetAdaptValue(&currentKeyFrameParam_.rotation);
    fovEase_.SetAdaptValue(&currentKeyFrameParam_.fov);

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

void CameraKeyFrame::Update(float deltaTime) {

  
    positionEase_.Update(deltaTime);
    rotationEase_.Update(deltaTime);
    fovEase_.Update(deltaTime);
}

void CameraKeyFrame::BindParams() {
    globalParameter_->Bind(groupName_, "timePoint", &timePoint_);
    globalParameter_->Bind(groupName_, "position", &keyFrameParam_.position);
    globalParameter_->Bind(groupName_, "rotation", &keyFrameParam_.rotation);
    globalParameter_->Bind(groupName_, "fov", &keyFrameParam_.fov);
    globalParameter_->Bind(groupName_, "positionEaseType", &positionEaseType_);
    globalParameter_->Bind(groupName_, "rotationEaseType", &rotationEaseType_);
    globalParameter_->Bind(groupName_, "fovEaseType", &fovEaseType_);
}

void CameraKeyFrame::AdjustParam() {
#ifdef _DEBUG

    ImGui::SeparatorText(("Camera KeyFrame: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    ImGui::DragFloat("Time Point", &timePoint_, 0.01f);

    ImGui::DragFloat3("Position", &keyFrameParam_.position.x, 0.1f);
    ImGui::DragFloat3("Rotation", &keyFrameParam_.rotation.x, 0.1f);

    ImGui::DragFloat("FOV", &keyFrameParam_.fov, 0.01f);

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

void CameraKeyFrame::EasingTypeSelector(const char* label, int32_t& target) {
    int type = static_cast<int>(target);
    if (ImGui::Combo(label, &type, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
        target = type;
    }
}