#include "DirectionalLight.h"

using namespace KetaEngine;
#include"Base/Dx/DirectXCommon.h"
#include "Pipeline/Object3D/Object3DPipeline.h"
#include <imgui.h>
#include "Vector3.h"
#include "Vector4.h"


void DirectionalLight::Init(ID3D12Device* device) {

    // ライトデータの初期化
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(DirectionalLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));

    lightData_->color = Vector4::kWHITE();
  
    // グローバルパラメータに登録
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void DirectionalLight::RegisterParams() {
    globalParameter_->Regist(groupName_, "Direction", &lightData_->direction);
    globalParameter_->Regist(groupName_, "Intensity", &lightData_->intensity);
}

void DirectionalLight::DebugImGui() {
    if (ImGui::CollapsingHeader("DirectionalLight")) {
        ImGui::DragFloat3("Direction", (float*)&lightData_->direction, 0.01f);
        ImGui::DragFloat("Intensity", (float*)&lightData_->intensity, 0.1f);
        lightData_->direction = lightData_->direction.Normalize();

        // 登録が切れてたら再登録
        if (!globalParameter_->HasRegisters(groupName_)) {
            RegisterParams();
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
    }
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

void  DirectionalLight::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(Object3DRootParameter::DirectionalLight), lightResource_->GetGPUVirtualAddress());
}
