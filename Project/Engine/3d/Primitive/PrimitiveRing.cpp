#include "PrimitiveRing.h"

using namespace KetaEngine;
#include "3d/Mesh.h"
#include "Base/Dx/DirectXCommon.h"
#include <numbers>
#include <vector>

void PrimitiveRing::Init() {
    vertexNum_ = params_.divisions * 6;
    IPrimitive::Init();
}

void PrimitiveRing::Rebuild() {
    RebuildInternal(params_.divisions * 6);
    Create();
}

void PrimitiveRing::Create() {
    const uint32_t kDivisions      = static_cast<uint32_t>(params_.divisions);
    const float radianPerDivide    = (2.0f * std::numbers::pi_v<float>) / float(kDivisions);

    for (uint32_t index = 0; index < kDivisions; ++index) {
        float sin     = std::sin(index       * radianPerDivide);
        float cos     = std::cos(index       * radianPerDivide);
        float sinNext = std::sin((index + 1) * radianPerDivide);
        float cosNext = std::cos((index + 1) * radianPerDivide);

        // UV.x = 正規化角度 (0-1) → 角度クリップに使用
        float u     = float(index)     / float(kDivisions);
        float uNext = float(index + 1) / float(kDivisions);

        uint32_t base = index * 6;

        // 三角形1: 外[i], 外[i+1], 内[i]
        mesh_->SetVertexPositionData(base + 0, Vector4(-sin     * params_.outerRadius, cos     * params_.outerRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(base + 0, Vector2(u,     0.0f));

        mesh_->SetVertexPositionData(base + 1, Vector4(-sinNext * params_.outerRadius, cosNext * params_.outerRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(base + 1, Vector2(uNext, 0.0f));

        mesh_->SetVertexPositionData(base + 2, Vector4(-sin     * params_.innerRadius, cos     * params_.innerRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(base + 2, Vector2(u,     1.0f));

        // 三角形2: 内[i], 外[i+1], 内[i+1]
        mesh_->SetVertexPositionData(base + 3, Vector4(-sin     * params_.innerRadius, cos     * params_.innerRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(base + 3, Vector2(u,     1.0f));

        mesh_->SetVertexPositionData(base + 4, Vector4(-sinNext * params_.outerRadius, cosNext * params_.outerRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(base + 4, Vector2(uNext, 0.0f));

        mesh_->SetVertexPositionData(base + 5, Vector4(-sinNext * params_.innerRadius, cosNext * params_.innerRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(base + 5, Vector2(uNext, 1.0f));
    }

    std::vector<uint32_t> indices(vertexNum_);
    for (uint32_t i = 0; i < vertexNum_; ++i) {
        indices[i] = i;
    }
    mesh_->SetIndexData(indices.data(), static_cast<uint32_t>(indices.size()));
}

