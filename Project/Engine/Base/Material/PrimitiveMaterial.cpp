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

void PrimitiveMaterial::SetAngleClipRange(float startDeg, float endDeg) {
    materialData_->startAngle      = startDeg / 360.0f;
    materialData_->endAngle        = endDeg   / 360.0f;
    materialData_->enableAngleClip = 1;
}

void PrimitiveMaterial::DebugImGui() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&materialData_->color));
    if (materialData_->enableAngleClip) {
        float start = materialData_->startAngle * 360.0f;
        float end   = materialData_->endAngle   * 360.0f;
        if (ImGui::DragFloat("StartAngle", &start, 1.0f, 0.0f, 360.0f)) {
            materialData_->startAngle = start / 360.0f;
        }
        if (ImGui::DragFloat("EndAngle", &end, 1.0f, 0.0f, 360.0f)) {
            materialData_->endAngle = end / 360.0f;
        }
    }
#endif
}
