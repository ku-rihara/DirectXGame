#include "SpriteMaterial.h"
#include "DirectXCommon.h"
#include <imgui.h>
#include <cassert>

SpriteMaterial::SpriteMaterial()
    : materialData_(nullptr) {  // materialData_の初期化
   
}

void SpriteMaterial::CreateMaterialResource(DirectXCommon* dxCommon) {
    assert(dxCommon);

    // マテリアルリソース作成
    materialResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(MaterialSprite));

    // マテリアルデータへのポインタ取得
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

    // 初期値をセット
    materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);  // 初期値を直接ここで設定
    materialData_->uvTransform = MakeIdentity4x4();

}

void SpriteMaterial::UpdateMaterialData(const Vector4& newColor) {
    materialData_->color = newColor;
}

void SpriteMaterial::SetCommandList(ID3D12GraphicsCommandList* commandList) {
    // シェーダーにマテリアルデータを送る
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
}

#ifdef _DEBUG
void SpriteMaterial::DebugImGui() {
    ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&materialData_->color));  // materialData_のcolorを使用
}
#endif
