

#include "AreaLight.h"
#include"Dx/DirectXCommon.h"
#include <imgui.h>
#include <numbers>


// 初期化
void AreaLight::Init(ID3D12Device* device) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(AreaLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));

    // 初期値設定
    lightData_->color = { 0.9943f,0.0773f,0.0773f, 1.0f };       // 色
    lightData_->position = { -5.3f, -1.8f, -3.1f };          // 中心位置
    lightData_->intensity = 1.0f;                         // 輝度
    lightData_->normal = { 0.0f, 1.0f, -0.03f };            // 法線
    lightData_->width = 6.4f;                             // 幅
    lightData_->height = 0.29f;                            // 高さ
    lightData_->up = { 0.0f, 0.0f, 1.0f };                // 上方向ベクトル
    lightData_->decay = 2.7f;                            // 減衰率
}

// ImGuiによるデバッグUI
void AreaLight::DebugImGui() {
    ImGui::ColorEdit4("Color", (float*)&lightData_->color);              // 色編集
    ImGui::DragFloat3("Position", (float*)&lightData_->position, 0.01f); // 位置編集
    ImGui::DragFloat3("Direction", (float*)&lightData_->normal, 0.01f);     // 法線編集
    lightData_->normal = Vector3::Normalize(lightData_->normal);         // 法線を正規化

    ImGui::DragFloat("Width", (float*)&lightData_->width, 0.01f);        // 幅編集
    ImGui::DragFloat("Height", (float*)&lightData_->height, 0.01f);      // 高さ編集
    ImGui::DragFloat3("Up", (float*)&lightData_->up, 0.01f);             // 上方向ベクトル編集
    lightData_->up = Vector3::Normalize(lightData_->up);                 // 上方向ベクトルを正規化

    ImGui::DragFloat("Intensity", (float*)&lightData_->intensity, 0.01f);// 輝度編集
    ImGui::DragFloat("Decay", (float*)&lightData_->decay, 0.01f);        // 減衰率編集
}

// ライトの位置を設定
void AreaLight::SetPosition(const Vector3& pos) {
    lightData_->position = pos;
}

// ライトデータをコマンドリストに設定
void AreaLight::SetLightCommand(ID3D12GraphicsCommandList* commandList, const int& index) {
    index; // 使用しないが引数として保持
    commandList->SetGraphicsRootConstantBufferView(8, lightResource_->GetGPUVirtualAddress()); // 定数バッファのビューを設定
}