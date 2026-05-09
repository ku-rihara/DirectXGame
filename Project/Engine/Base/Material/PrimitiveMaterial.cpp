#include "PrimitiveMaterial.h"
#include "Base/Dx/DirectXCommon.h"
#include <cassert>
#include <imgui.h>

using namespace KetaEngine;

void PrimitiveMaterial::Init(DirectXCommon* dxCommon) {
    assert(dxCommon);
    materialResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(MaterialData));
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    *materialData_ = MaterialData{};
}

void PrimitiveMaterial::UpdateMaterialData(const Vector4& color) {
    materialData_->color = color;
}

void PrimitiveMaterial::SetCommandList(ID3D12GraphicsCommandList* commandList) {
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
}

void PrimitiveMaterial::DebugImGui() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&materialData_->color));
#endif
}
