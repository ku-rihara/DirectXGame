#include "AmbientLight.h"
#include "base/DirectXCommon.h"
#include <imgui.h>
#include "Vector3.h"
#include "Vector4.h"

AmbientLight::AmbientLight()
{
    lightResource_ = nullptr;
    lightData_ = nullptr;
}
AmbientLight::~AmbientLight()
{
    if (lightResource_)
    {
        lightResource_->Release();
        lightResource_ = nullptr;
    }
    lightData_ = nullptr;
}
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
        // ImGui を使って環境ライトのパラメータを調整
        ImGui::ColorEdit4("Color", (float*)&lightData_->color);
        ImGui::DragFloat("Intensity", &lightData_->intensity, 0.1f, 0.0f);
        ImGui::PopID();
    }
}

void  AmbientLight::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootConstantBufferView(8, lightResource_->GetGPUVirtualAddress());
}