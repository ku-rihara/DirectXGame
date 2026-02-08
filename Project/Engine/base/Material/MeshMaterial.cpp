#include "Base/Dx/DirectXCommon.h"
#include "MeshMaterial.h"

using namespace KetaEngine;
#include "Light/LightingType.h"
#include <cassert>
#include <imgui.h>

void MeshMaterial::Init(DirectXCommon* dxCommon) {
    assert(dxCommon);

    // マテリアルリソース作成
    materialResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(MaterialData));

    // マテリアルデータへのポインタ取得
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

    // 初期値をセット
    materialData_->color                  = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData_->shininess              = 9.5f;
    materialData_->uvMatrix               = MakeIdentity4x4();
    materialData_->enableLighting         = static_cast<int32_t>(LightingType::Lambert);
    materialData_->environmentCoefficient = 0.0f;
}

void MeshMaterial::UpdateMaterialData(const Vector4& Color) {
    materialData_->color = Color;
}

void MeshMaterial::SetCommandList(ID3D12GraphicsCommandList* commandList) {
    // シェーダーにマテリアルデータを送る
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
}

void MeshMaterial::DebugImGui() {
#ifdef _DEBUG
    ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&materialData_->color));
    ImGui::DragFloat("Shininess", &materialData_->shininess, 0.01f);
    ImGui::DragFloat("environmentCoefficient", &materialData_->environmentCoefficient, 0.01f);
    const char* lightingModes[] = {"No Lighting", "Lambert", "Half Lambert", "Specular Reflection",
        "PointLight", "SpotLight", "AreaLight", "Ambient"};
    ImGui::Combo("Lighting Mode", &materialData_->enableLighting, lightingModes, IM_ARRAYSIZE(lightingModes));
#endif
}
