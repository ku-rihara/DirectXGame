#include "Material.h"
#include "DirectXCommon.h"
#include <imgui.h>
#include <cassert>

Material::Material() : color(Vector4(1.0f, 1.0f, 1.0f, 1.0f)), shininess(0.5f), uvTransform(MakeIdentity4x4()), enableLighting(1) {
}

void Material::CreateMaterialResource(DirectXCommon* dxCommon) {
    assert(dxCommon);

    // マテリアルリソース作成
    materialResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(Material));

    // マテリアルデータへのポインタ取得
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

    // 初期値をセット
    materialData_->color = color;
    materialData_->shininess = shininess;
    materialData_->uvTransform = uvTransform;
    materialData_->enableLighting = enableLighting;
}

void Material::UpdateMaterialData(const Vector4& newColor) {
    materialData_->color = newColor;
}

void Material::SetToShader(ID3D12GraphicsCommandList* commandList) {
    // シェーダーにマテリアルデータを送る
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
}

#ifdef _DEBUG
void Material::DebugImGui() {
    ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&color));
    ImGui::DragFloat("Shininess", &shininess, 0.01f);
    const char* lightingModes[] = { "No Lighting", "Lambert", "Half Lambert","Specular Reflection","PointLight","SpotLight" };
    ImGui::Combo("Lighting Mode", &enableLighting, lightingModes, IM_ARRAYSIZE(lightingModes));
}
#endif
