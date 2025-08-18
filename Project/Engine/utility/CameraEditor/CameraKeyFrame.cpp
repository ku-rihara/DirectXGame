#include "CameraKeyFrame.h"
#include <imgui.h>

void CameraKeyFrame::Init(const std::string& cameraAnimationName, const int32_t& keyNumber) {
    // グローバルパラメータ
    globalParameter_     = GlobalParameter::GetInstance();
    currentKeyFrameIndex = keyNumber;
    groupName_           = "CameraAnimation/" + cameraAnimationName + "_KeyFrame_" + std::to_string(currentKeyFrameIndex);
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // parameterをEasingに適応
    positionEase_.SetMaxTime(timePoint_);
    rotationEase_.SetMaxTime(timePoint_);
    fovEase_.SetMaxTime(timePoint_);

    positionEase_.SetType(static_cast<EasingType>(positionEaseType_));
    rotationEase_.SetType(static_cast<EasingType>(rotationEaseType_));
    fovEase_.SetType(static_cast<EasingType>(fovEaseType_));

    // adapt
    positionEase_.SetAdaptValue(&position_);
    rotationEase_.SetAdaptValue(&rotation_);
    fovEase_.SetAdaptValue(&fov_);
}
void CameraKeyFrame::Update(float deltaTime) {
    positionEase_.Update(deltaTime);
    rotationEase_.Update(deltaTime);
    fovEase_.Update(deltaTime);
}

void CameraKeyFrame::BindParams() {
    globalParameter_->Bind(groupName_, "timePoint", &timePoint_);
    globalParameter_->Bind(groupName_, "position", &position_);
    globalParameter_->Bind(groupName_, "rotation", &rotation_);
    globalParameter_->Bind(groupName_, "fov", &fov_);
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

        ImGui::DragFloat("Time Point", &timePoint_, 0.01f, 0.0f, 9999.0f);

        ImGui::DragFloat3("Position", &position_.x, 0.1f);
        ImGui::DragFloat3("Rotation", &rotation_.x, 0.1f);

        ImGui::DragFloat("FOV", &fov_, 0.01f);

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

        std::string lbl(label);
        if (lbl == "Easing Type Position") {
            positionEase_.SetType(static_cast<EasingType>(target));
        } else if (lbl == "Easing Type Rotate") {
            rotationEase_.SetType(static_cast<EasingType>(target));
        } else if (lbl == "Easing Type Fov") {
            fovEase_.SetType(static_cast<EasingType>(target));
        }
    }
}