#include "CameraKeyFrame.h"
#include <imgui.h>

void CameraKeyFrame::Init() {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // parameterをEasingに適応
    positionEase_.SetMaxTime(timePoint);
    rotationEase_.SetMaxTime(timePoint);
    fovEase_.SetMaxTime(timePoint);

    positionEase_.SetType(static_cast<EasingType>(positionEaseType_));
    rotationEase_.SetType(static_cast<EasingType>(rotationEaseType_));
    fovEase_.SetType(static_cast<EasingType>(fovEaseType_));
}
void CameraKeyFrame::Update(float deltaTime) {
}

void CameraKeyFrame::BindParams() {
    globalParameter_->Bind(groupName_, "timePoint", &timePoint);
    globalParameter_->Bind(groupName_, "position", &position);
    globalParameter_->Bind(groupName_, "rotation", &rotation);
    globalParameter_->Bind(groupName_, "fov", &fov);
    globalParameter_->Bind(groupName_, "positionEaseType", &positionEaseType_);
    globalParameter_->Bind(groupName_, "rotationEaseType", &rotationEaseType_);
    globalParameter_->Bind(groupName_, "fovEaseType", &fovEaseType_);
}

void CameraKeyFrame::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::DragFloat("Time Point", &timePoint, 0.01f, 0.0f, 9999.0f);

        ImGui::DragFloat3("Position", &position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);

        ImGui::DragFloat("FOV", &fov, 0.01f);

        // イージングタイプの設定
        EasingTypeSelector("Easing Type Position", positionEaseType_);
        EasingTypeSelector("Easing Type Rotate", rotationEaseType_);
        EasingTypeSelector("Easing Type Fov", fovEaseType_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void CameraKeyFrame::EasingTypeSelector(const char* label, int32_t& target) {
    int type = static_cast<int>(target);
    if (ImGui::Combo(label, &type, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
        target = type;

        if (label == "Easing Type Position") {
            positionEase_.SetType(static_cast<EasingType>(target));
        } else if (label == "Easing Type Rotate") {
            rotationEase_.SetType(static_cast<EasingType>(target));
        } else if (label == "Easing Type Fov") {
            fovEase_.SetType(static_cast<EasingType>(target));
        }
    }
}