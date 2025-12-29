#include "AreaLight.h"

using namespace KetaEngine;
#include"Dx/DirectXCommon.h"
#include <imgui.h>
#include <numbers>


// 初期化
void AreaLight::Init(ID3D12Device* device) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(AreaLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));

    // 初期値設定
    lightData_->color = { 0.9943f,0.0773f,0.0773f, 1.0f }; 
    lightData_->position = { -5.3f, -1.8f, -3.1f };        
    lightData_->intensity = 1.0f;                         
    lightData_->normal = { 0.0f, 1.0f, -0.03f };           
    lightData_->width = 6.4f;                             
    lightData_->height = 0.29f;                            
    lightData_->up = { 0.0f, 0.0f, 1.0f };                
    lightData_->decay = 2.7f;                            
}

// ImGuiによるデバッグUI
void AreaLight::DebugImGui() {
    ImGui::ColorEdit4("Color", (float*)&lightData_->color);              
    ImGui::DragFloat3("Position", (float*)&lightData_->position, 0.01f); 
    ImGui::DragFloat3("Direction", (float*)&lightData_->normal, 0.01f);  
    lightData_->normal = lightData_->normal.Normalize();         

    ImGui::DragFloat("Width", (float*)&lightData_->width, 0.01f);        
    ImGui::DragFloat("Height", (float*)&lightData_->height, 0.01f);      
    ImGui::DragFloat3("Up", (float*)&lightData_->up, 0.01f);             
    lightData_->up = lightData_->up.Normalize();                 

    ImGui::DragFloat("Intensity", (float*)&lightData_->intensity, 0.01f);
    ImGui::DragFloat("Decay", (float*)&lightData_->decay, 0.01f);        
}

// ライトの位置を設定
void AreaLight::SetPosition(const Vector3& pos) {
    lightData_->position = pos;
}

// ライトデータをコマンドリストに設定
void AreaLight::SetLightCommand(ID3D12GraphicsCommandList* commandList, const int& index) {
    index; // 使用しないが引数として保持
    commandList->SetGraphicsRootConstantBufferView(8, lightResource_->GetGPUVirtualAddress()); 
}
