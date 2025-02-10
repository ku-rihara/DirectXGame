#include "Material.h"
#include "DirectXCommon.h"
#include <imgui.h>
#include <cassert>

Material::Material()
    : materialData_(nullptr) {  // materialData_の初期化
}

void Material::CreateMaterialResource(DirectXCommon* dxCommon) {
    assert(dxCommon);

    // マテリアルリソース作成
    materialResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(MaterialStructure));  // sizeof(MaterialStructure)を使用

    // マテリアルデータへのポインタ取得
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

    // 初期値をセット
    materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);  // 初期値を直接ここで設定
    materialData_->shininess = 12.5f;
    materialData_->uvTransform = MakeIdentity4x4();
    materialData_->enableLighting = 2;
}

void Material::UpdateMaterialData(const Vector4& Color) {
    materialData_->color = Color;
}

void Material::SetCommandList(ID3D12GraphicsCommandList* commandList) {
    // シェーダーにマテリアルデータを送る
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
}

#ifdef _DEBUG
void Material::DebugImGui() {
    ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&materialData_->color));  // materialData_のcolorを使用
    ImGui::DragFloat("Shininess", &materialData_->shininess, 0.01f);  // materialData_のshininessを使用
    const char* lightingModes[] = { "No Lighting", "Lambert", "Half Lambert", "Specular Reflection",
                                    "PointLight", "SpotLight","AreaLight","Ambient"};
    ImGui::Combo("Lighting Mode", &materialData_->enableLighting, lightingModes, IM_ARRAYSIZE(lightingModes));  // materialData_のenableLightingを使用
}
#endif
