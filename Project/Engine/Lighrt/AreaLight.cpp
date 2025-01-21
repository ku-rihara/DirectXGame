#include "AreaLight.h"
#include "base/DirectXCommon.h"
#include <imgui.h>
#include <numbers>

AreaLight::AreaLight()
{
    lightResource_ = nullptr;
    lightData_ = nullptr;
}

AreaLight::~AreaLight()
{
    if (lightResource_)
    {
        lightResource_->Release();
        lightResource_ = nullptr;
    }
    lightData_ = nullptr;
}

void AreaLight::Init(ID3D12Device* device) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(AreaLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));

    // 初期値設定
    lightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    lightData_->position = { 0.0f, 1.0f, 0.0f };
    lightData_->intensity = 1.0f;
    lightData_->normal = { 0.0f, 1.0f, 0.0f };
    lightData_->size = { 2.0f, 0.2f, 2.0f };
    lightData_->decay = 0.15f;

}

void AreaLight::DebugImGui() {
    ImGui::ColorEdit4("Color", (float*)&lightData_->color);
    ImGui::DragFloat3("Position", (float*)&lightData_->position, 0.01f);
    ImGui::DragFloat3("Normal", (float*)&lightData_->normal, 0.01f);
    lightData_->normal = Vector3::Normalize(lightData_->normal);
    ImGui::DragFloat3("Size", (float*)&lightData_->size, 0.01f);
    ImGui::DragFloat("Intensity", (float*)&lightData_->intensity, 0.01f);
    ImGui::DragFloat("Decay", (float*)&lightData_->decay, 0.01f);

}

void AreaLight::SetPosition(const Vector3& pos) {
    lightData_->position = pos;
}

void AreaLight::SetLightCommand(ID3D12GraphicsCommandList* commandList, const int& index) {
    index;
    commandList->SetGraphicsRootConstantBufferView(7, lightResource_->GetGPUVirtualAddress()); // スポットライトのルートパラメータの次の番号を指定
}