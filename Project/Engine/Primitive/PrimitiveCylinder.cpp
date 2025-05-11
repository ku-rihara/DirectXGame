#include "PrimitiveCylinder.h"
#include "3d/Mesh.h"
#include <numbers>

void PrimitiveCylinder::Init() {
    vertexNum_ = 32;
    IPrimitive::Init();
}

void PrimitiveCylinder::Create() {

    const float kTopRadius      = 1.0f;
    const float kBottomRadius   = 1.0f;
    const float kHeight         = 3.0f;
    const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(vertexNum_);

    for (uint32_t index = 0; index < vertexNum_; ++index) {
        float sin     = std::sin(index * radianPerDivide);
        float cos     = std::cos(index * radianPerDivide);
        float sinNext = std::sin((index + 1) * radianPerDivide);
        float cosNext = std::cos((index + 1) * radianPerDivide);
        float u       = float(index) / float(vertexNum_);
        float uNext   = float(index + 1) / float(vertexNum_);

        // positionとuv
        mesh_->SetVertexPositionData(index, Vector4(-sin * kTopRadius, kHeight, cos * kTopRadius, 1.0f));
        mesh_->SetVertexTexcoordData(index, Vector2(u, 0.0f));
        mesh_->SetVertexNormData(index, Vector3(-sin, 0.0f, cos));

        mesh_->SetVertexPositionData(index + 1, Vector4(-sinNext * kTopRadius, kHeight, cosNext * kTopRadius, 1.0f));
        mesh_->SetVertexTexcoordData(index + 1, Vector2(uNext, 0.0f));
        mesh_->SetVertexNormData(index + 1, Vector3(-sinNext, 0.0f, cosNext));

        mesh_->SetVertexPositionData(index + 2, Vector4(-sin * kBottomRadius, 0.0f, cos * kBottomRadius, 1.0f));
        mesh_->SetVertexTexcoordData(index + 2, Vector2(u, 1.0f));
        mesh_->SetVertexNormData(index + 2, Vector3(-sin, 0.0f, cos));

        mesh_->SetVertexPositionData(index + 3, Vector4(-sin * kBottomRadius, 0.0f, cos * kBottomRadius, 1.0f));
        mesh_->SetVertexTexcoordData(index + 3, Vector2(u, 1.0f));
        mesh_->SetVertexNormData(index + 3, Vector3(-sin, 0.0f, cos));

        mesh_->SetVertexPositionData(index + 4, Vector4(-sinNext * kTopRadius, kHeight, cosNext * kTopRadius, 1.0f));
        mesh_->SetVertexTexcoordData(index + 4, Vector2(uNext, 0.0f));
        mesh_->SetVertexNormData(index + 4, Vector3(-sinNext, 0.0f, cosNext));

        mesh_->SetVertexPositionData(index + 5, Vector4(-sinNext * kBottomRadius, 0.0f, cosNext * kBottomRadius, 1.0f));
        mesh_->SetVertexTexcoordData(index + 5, Vector2(uNext, 1.0f));
        mesh_->SetVertexNormData(index + 5, Vector3(-sinNext, 0.0f, cosNext));
    }
}

void PrimitiveCylinder::SetTexture(const std::string& name) {
    IPrimitive::SetTexture(name);
}

void PrimitiveCylinder::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    IPrimitive::Draw(worldTransform, viewProjection, textureHandle);
}