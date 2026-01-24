#include "GPUParticleMaterial.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include <cassert>
#include <imgui.h>

void GPUParticleMaterial::CreateMaterialResource(DirectXCommon* dxCommon) {
    assert(dxCommon);

    // マテリアルリソース作成
    materialResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(MaterialData));
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

    // 初期値をセット
    materialData_->color                  = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData_->shininess              = 9.5f;
    materialData_->uvMatrix               = MakeIdentity4x4();
    materialData_->enableLighting         = 1;
    materialData_->environmentCoefficient = 0.0f;

    // UVアニメーションリソース作成
    uvAnimeResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(UVAnimationData));
    uvAnimeResource_->Map(0, nullptr, reinterpret_cast<void**>(&uvAnimeData_));

}

void GPUParticleMaterial::UpdateMaterialData(const Vector4& Color) {
    materialData_->color = Color;
}

void GPUParticleMaterial::UpdateUVAnimation(float deltaTime) {
    uvAnimeData_->currentTime += deltaTime;

}

void GPUParticleMaterial::SetCommandList(ID3D12GraphicsCommandList* commandList) {
    // マテリアルデータ
    commandList->SetGraphicsRootConstantBufferView(2, materialResource_->GetGPUVirtualAddress());

    // UVアニメーションデータ
    commandList->SetGraphicsRootConstantBufferView(3, uvAnimeResource_->GetGPUVirtualAddress());
}

void GPUParticleMaterial::DebugImGui() {
#ifdef _DEBUG
    ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&materialData_->color));
    ImGui::DragFloat("Shininess", &materialData_->shininess, 0.01f);
    ImGui::DragFloat("environmentCoefficient", &materialData_->environmentCoefficient, 0.01f);
    const char* lightingModes[] = {"No Lighting", "Lambert", "Half Lambert", "Specular Reflection",
        "PointLight", "SpotLight", "AreaLight", "Ambient"};
    ImGui::Combo("Lighting Mode", &materialData_->enableLighting, lightingModes, IM_ARRAYSIZE(lightingModes));
#endif
}
