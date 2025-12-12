#include "SpotLight.h"

using namespace KetaEngine;
#include "Dx/DirectXCommon.h"
#include <imgui.h>

void SpotLight::Init(ID3D12Device* device, const std::string& groupName) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(SpotLightData));
    lightData_     = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));

    groupName_       = groupName;
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void SpotLight::Update() {
    if (isMove_) {
        return;
    }
    lightData_->position = tempPos_;
 }

void SpotLight::DebugImGui() {
}

void SpotLight::RegisterParams() {
    lightData_->decay = max(lightData_->decay,0.1f);
    globalParameter_->Regist(groupName_, "Color", &lightData_->color);
    globalParameter_->Regist(groupName_, "Pos", &tempPos_);
    globalParameter_->Regist(groupName_, "Direction", &lightData_->direction);
    globalParameter_->Regist(groupName_, "Distance", &lightData_->distance);
    globalParameter_->Regist(groupName_, "intenesity", &lightData_->intensity);
    globalParameter_->Regist(groupName_, "decay", &lightData_->decay);
    globalParameter_->Regist(groupName_, "cosAngle", &lightData_->cosAngle);
    globalParameter_->Regist(groupName_, "cosFalloffStart", &lightData_->cosFalloffStart);
    globalParameter_->Regist(groupName_, "isMove", &isMove_);
}

void SpotLight::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());
        ImGui::Checkbox("IsMove", &isMove_);
        ImGui::ColorEdit4(" Color", (float*)&lightData_->color);
        ImGui::DragFloat3(" Pos", (float*)&tempPos_, 0.01f);
        ImGui::DragFloat3(" Direction", (float*)&lightData_->direction, 0.01f);
        lightData_->direction = Vector3::Normalize(lightData_->direction);
        ImGui::DragFloat("  Distance", (float*)&lightData_->distance, 0.01f);
        ImGui::DragFloat("  intenesity", (float*)&lightData_->intensity, 0.01f);
        ImGui::DragFloat("  decay", (float*)&lightData_->decay, 0.01f);
        ImGui::DragFloat("  cosAngle", (float*)&lightData_->cosAngle, 0.01f);
        ImGui::DragFloat("  cosFalloffStart", (float*)&lightData_->cosFalloffStart, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void SpotLight::SetPosition(const Vector3& pos) {
    lightData_->position = pos;
}
