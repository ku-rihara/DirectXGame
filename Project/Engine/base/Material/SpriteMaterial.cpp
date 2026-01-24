#include "SpriteMaterial.h"

using namespace KetaEngine;
#include"Base/Dx/DirectXCommon.h"
#include <imgui.h>
#include <cassert>

SpriteMaterial::SpriteMaterial()
    : materialData_(nullptr) {  
   
}

void SpriteMaterial::CreateMaterialResource(DirectXCommon* dxCommon) {
    assert(dxCommon);

    // マテリアルリソース作成
    materialResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(MaterialSprite));

    // マテリアルデータへのポインタ取得
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

    // 初期値をセット
    materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f); 
    materialData_->uvTransform = MakeIdentity4x4();

}

void SpriteMaterial::UpdateMaterialData(const Vector4& newColor) {
    materialData_->color = newColor;
}

void SpriteMaterial::SetCommandList(ID3D12GraphicsCommandList* commandList) {
    // シェーダーにマテリアルデータを送る
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
}


void SpriteMaterial::DebugImGui() {
#ifdef _DEBUG
    ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&materialData_->color));  
#endif
}

