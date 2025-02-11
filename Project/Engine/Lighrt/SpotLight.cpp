#include "SpotLight.h"
#include "base/DirectXCommon.h"
#include <imgui.h>
#include <numbers>

SpotLight::SpotLight()
{
    lightResource_ = nullptr;
    lightData_ = nullptr;
}
SpotLight::~SpotLight()
{
   
}

void SpotLight::Init(ID3D12Device* device) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(SpotLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
    lightData_->color = {0.8235f, 0.5882f, 0.2078f,1.0f };
    lightData_->position = { 2.3f,5.0f,0.0f };
    lightData_->distance = 7.0f;
    lightData_->direction = Vector3::Normalize({ 0.0f,-1.0f,0.0f });
    lightData_->intensity = 5.0f;
    lightData_->decay = 0.05f;
    lightData_->cosAngle = 0.5f;
    lightData_->cosFalloffStart = 1.5f;

}

void SpotLight::DebugImGui() {
    ImGui::ColorEdit4(" Color", (float*)&lightData_->color);
    ImGui::DragFloat3(" Pos", (float*)&lightData_->position, 0.01f);
    ImGui::DragFloat3(" Direction", (float*)&lightData_->direction, 0.01f);
    lightData_->direction = Vector3::Normalize(lightData_->direction);
    ImGui::DragFloat("  Distance", (float*)&lightData_->distance, 0.01f);
    ImGui::DragFloat("  intenesity", (float*)&lightData_->intensity, 0.01f);
    ImGui::DragFloat("  decay", (float*)&lightData_->decay, 0.01f);
    ImGui::DragFloat("  cosAngle", (float*)&lightData_->cosAngle, 0.01f);
    ImGui::DragFloat("  cosFalloffStart", (float*)&lightData_->cosFalloffStart, 0.01f);
}

void SpotLight::SetPosition(const Vector3& pos) {
    lightData_->position = pos;
}

void  SpotLight::SetLightCommand(ID3D12GraphicsCommandList* commandList,const int&index) {
    index;
    commandList->SetGraphicsRootConstantBufferView(6, lightResource_->GetGPUVirtualAddress());
}