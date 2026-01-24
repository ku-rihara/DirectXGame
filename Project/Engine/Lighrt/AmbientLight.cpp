#include "AmbientLight.h"

using namespace KetaEngine;
#include"Pipeline/Object3D/Object3DPipeline.h"
#include"Base/Dx/DirectXCommon.h"
#include <imgui.h>
#include "Vector3.h"
#include "Vector4.h"


void AmbientLight::Init(ID3D12Device* device) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(AmbientLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
    // 初期化
    lightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    lightData_->intensity = 1.0f;
    
}


void AmbientLight::SetColor(const Vector4& color) {
    lightData_->color = color;
}
void AmbientLight::SetIntensity(float intensity) {
    lightData_->intensity = intensity;
}
void AmbientLight::DebugImGui(){

    if (ImGui::CollapsingHeader("AmbientLight")){
        ImGui::PushID("AmbientLight");
        // パラメータを調整      
        ImGui::DragFloat("Intensity", &lightData_->intensity, 0.01f, 0.0f, 10.0f);
        ImGui::ColorEdit4("Color", (float*)&lightData_->color);
        ImGui::PopID();
    }
}

void  AmbientLight::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(Object3DRootParameter::AmbientLight), lightResource_->GetGPUVirtualAddress());
}
