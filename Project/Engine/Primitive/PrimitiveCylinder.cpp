#include "PrimitiveCylinder.h"

using namespace KetaEngine;
#include "3d/Mesh.h"
#include <numbers>
#include <vector>

void PrimitiveCylinder::Init() {
    vertexNum_ = 32*6;
    IPrimitive::Init();
}

void PrimitiveCylinder::Create() {
    const float kTopRadius      = 1.0f;
    const float kBottomRadius   = 1.0f;
    const float kHeight         = 3.0f;
    const float radianPerDivide = (12.0f * std::numbers::pi_v<float>) / float(vertexNum_);

    for (uint32_t index = 0; index < vertexNum_; ++index) {
        float sin     = std::sin(index * radianPerDivide);
        float cos     = std::cos(index * radianPerDivide);
        float sinNext = std::sin((index + 1) * radianPerDivide);
        float cosNext = std::cos((index + 1) * radianPerDivide);
        float u       = float(index) / float(vertexNum_);
        float uNext   = float(index + 1) / float(vertexNum_);

        // 各面は2つの三角形 → 6頂点
        uint32_t baseIndex = index * 6;

        mesh_->SetVertexPositionData(baseIndex + 0, Vector4(-sin * kTopRadius, kHeight, cos * kTopRadius, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 0, Vector2(u, 0.0f));
        mesh_->SetVertexNormData(baseIndex + 0, Vector3(-sin, 0.0f, cos));

        mesh_->SetVertexPositionData(baseIndex + 1, Vector4(-sinNext * kTopRadius, kHeight, cosNext * kTopRadius, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 1, Vector2(uNext, 0.0f));
        mesh_->SetVertexNormData(baseIndex + 1, Vector3(-sinNext, 0.0f, cosNext));

        mesh_->SetVertexPositionData(baseIndex + 2, Vector4(-sin * kBottomRadius, 0.0f, cos * kBottomRadius, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 2, Vector2(u, 1.0f));
        mesh_->SetVertexNormData(baseIndex + 2, Vector3(-sin, 0.0f, cos));

        mesh_->SetVertexPositionData(baseIndex + 3, Vector4(-sin * kBottomRadius, 0.0f, cos * kBottomRadius, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 3, Vector2(u, 1.0f));
        mesh_->SetVertexNormData(baseIndex + 3, Vector3(-sin, 0.0f, cos));

        mesh_->SetVertexPositionData(baseIndex + 4, Vector4(-sinNext * kTopRadius, kHeight, cosNext * kTopRadius, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 4, Vector2(uNext, 0.0f));
        mesh_->SetVertexNormData(baseIndex + 4, Vector3(-sinNext, 0.0f, cosNext));

        mesh_->SetVertexPositionData(baseIndex + 5, Vector4(-sinNext * kBottomRadius, 0.0f, cosNext * kBottomRadius, 1.0f));
        mesh_->SetVertexTexcoordData(baseIndex + 5, Vector2(uNext, 1.0f));
        mesh_->SetVertexNormData(baseIndex + 5, Vector3(-sinNext, 0.0f, cosNext));
    }

    // indexデータ生成
    std::vector<uint32_t> indices(vertexNum_);
    for (uint32_t i = 0; i < vertexNum_; ++i) {
        indices[i] = i;
    }
    mesh_->SetIndexData(indices.data(), static_cast<uint32_t>(indices.size()));
}


void PrimitiveCylinder::SetTexture(const std::string& name) {
    IPrimitive::SetTexture(name);
}

void PrimitiveCylinder::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    IPrimitive::Draw(worldTransform, viewProjection, textureHandle);
}
