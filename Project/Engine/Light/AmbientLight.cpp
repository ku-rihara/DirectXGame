#include "AmbientLight.h"

using namespace KetaEngine;
#include"Pipeline/Object3D/Object3DPipeline.h"
#include"Base/Dx/DirectXCommon.h"
#include <imgui.h>
#include "Vector4.h"


void AmbientLight::Init(ID3D12Device* device) {
    lightResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device, sizeof(AmbientLightData));
    lightData_ = nullptr;
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
    // 初期化
    lightData_->color = { 0.396f, 0.796f, 0.180f, 1.0f };
    lightData_->intensity = 0.9f;

    // グローバルパラメータに登録
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void AmbientLight::RegisterParams() {
    globalParameter_->Regist(groupName_, "Color", &lightData_->color);
    globalParameter_->Regist(groupName_, "Intensity", &lightData_->intensity);
}


void AmbientLight::SetColor(const Vector4& color) {
    lightData_->color = color;
}
void AmbientLight::SetIntensity(float intensity) {
    lightData_->intensity = intensity;
}
void AmbientLight::DebugImGui(){

    if (ImGui::CollapsingHeader("AmbientLight")){
        ImGui::PushID(groupName_.c_str());
        // パラメータを調整
        ImGui::DragFloat("Intensity", &lightData_->intensity, 0.01f, 0.0f, 10.0f);
        ImGui::ColorEdit4("Color", (float*)&lightData_->color);

        // 登録が切れてたら再登録
        if (!globalParameter_->HasRegisters(groupName_)) {
            RegisterParams();
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
}

void  AmbientLight::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(Object3DRootParameter::AmbientLight), lightResource_->GetGPUVirtualAddress());
}
