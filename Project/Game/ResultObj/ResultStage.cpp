#include "ResultStage.h"
#include <imgui.h>

void ResultStage::Init(const std::string& filename) {
    objectFromBlender_ = std::make_unique<KetaEngine::ObjectFromBlender>();
    objectFromBlender_->LoadJsonFile(filename);

    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void ResultStage::Update() {
    if (viewProjection_) {
        viewProjection_->translation_ = cameraPos_;
        viewProjection_->rotation_    = cameraRot_;
    }
}

void ResultStage::RegisterParams() {
    globalParameter_->Regist(groupName_, "cameraPos", &cameraPos_);
    globalParameter_->Regist(groupName_, "cameraRot", &cameraRot_);
}

void ResultStage::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());
        ImGui::DragFloat3("CameraPos", &cameraPos_.x, 0.1f);
        ImGui::DragFloat3("CameraRot", &cameraRot_.x, 0.01f);
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
#endif
}
