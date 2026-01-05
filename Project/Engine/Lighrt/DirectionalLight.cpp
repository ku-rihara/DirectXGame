#include "DirectionalLight.h"

using namespace KetaEngine;
#include"Pipeline/Object3D/Object3DPipeline.h"
#include"Dx/DirectXCommon.h"
#include <imgui.h>
#include "Vector3.h"
#include "Vector4.h"


void DirectionalLight::Init(ID3D12Device* device) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(DirectionalLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
    lightData_->color = { 1.0f,1.0f,1.0f,1.0f };
    lightData_->direction = { 0.0f,-1.0f,0.0f };
    lightData_->intensity = 1.0f;
}

void DirectionalLight::SetDirection(const Vector3& direction) {
    lightData_->direction = direction;
}
void DirectionalLight::SetColor(const Vector4& color) {
    lightData_->color = color;
}
void DirectionalLight::SetIntensity(float intensity) {
    lightData_->intensity = intensity;
}
void DirectionalLight::DebugImGui() {
    if (ImGui::CollapsingHeader("DirectionalLight")) {
        ImGui::DragFloat3("Direction", (float*)&lightData_->direction, 0.01f);
        ImGui::DragFloat("Intensity", (float*)&lightData_->intensity, 0.1f);
        lightData_->direction = lightData_->direction.Normalize();
       
    }
}

void  DirectionalLight::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(Object3DRootParameter::DirectionalLight), lightResource_->GetGPUVirtualAddress());
}
