#include "PrimitiveRing.h"
#include "3d/Mesh.h"
#include <numbers>
#include <vector>

void PrimitiveRing::Init() {
    vertexNum_ = 32*6;
    IPrimitive::Init();
}

void PrimitiveRing::Create() {
    const float kOuterRadius    = 1.0f;
    const float kInnerRadius    = 0.2f;
    const float radianPerDivide = (12.0f* std::numbers::pi_v<float>) / float(vertexNum_);

    for (uint32_t index = 0; index < vertexNum_; ++index) {
        float sin     = std::sin(index * radianPerDivide);
        float cos     = std::cos(index * radianPerDivide);
        float sinNext = std::sin((index + 1) * radianPerDivide);
        float cosNext = std::cos((index + 1) * radianPerDivide);
        float u       = float(index) / float(vertexNum_);
        float uNext   = float(index + 1) / float(vertexNum_);

        uint32_t baseIndex = index * 3;

        mesh_->SetVertexPositionData(baseIndex + 0, Vector4(-sin * kOuterRadius, cos * kOuterRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 0, Vector2(u, 0.0f));

        mesh_->SetVertexPositionData(baseIndex + 1, Vector4(-sinNext * kOuterRadius, cosNext * kOuterRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 1, Vector2(uNext, 0.0f));

        mesh_->SetVertexPositionData(baseIndex + 2, Vector4(-sin * kInnerRadius, cos * kInnerRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 2, Vector2(u, 1.0f));

        mesh_->SetVertexPositionData(baseIndex + 3, Vector4(-sinNext * kInnerRadius, cos * kInnerRadius, 0.0f, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 3, Vector2(uNext, 1.0f));
    }

    // indexデータ生成
    std::vector<uint32_t> indices(vertexNum_);
    for (uint32_t i = 0; i < vertexNum_; ++i) {
        indices[i] = i;
    }
    mesh_->SetIndexData(indices.data(), static_cast<uint32_t>(indices.size()));
}


void PrimitiveRing::SetTexture(const std::string& name) {
    IPrimitive::SetTexture(name);
}

void PrimitiveRing::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    IPrimitive::Draw(worldTransform, viewProjection, textureHandle);
}