#include "PointLight.h"
#include"Dx/DirectXCommon.h"
#include <imgui.h>


void PointLight::Init(ID3D12Device* device) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(PointLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
    lightData_->intenesity = 1.0f;
    lightData_->position.y = 2.0f;
}

void PointLight::DebugImGui() {
    ImGui::ColorEdit4("Color", (float*)&lightData_->color);
    ImGui::DragFloat3("Pos", (float*)&lightData_->position, 0.01f);
    ImGui::DragFloat("intenesity", (float*)&lightData_->intenesity, 0.01f);
    ImGui::DragFloat("radius", (float*)&lightData_->radius, 0.01f);
    ImGui::DragFloat("decay", (float*)&lightData_->decay, 0.01f);
}

void PointLight::SetPosition(const Vector3& pos) {
    lightData_->position = pos;
}

void  PointLight::SetLightCommand(ID3D12GraphicsCommandList* commandList, const int& index) {
    index;
    commandList->SetGraphicsRootConstantBufferView(6, lightResource_->GetGPUVirtualAddress());
}