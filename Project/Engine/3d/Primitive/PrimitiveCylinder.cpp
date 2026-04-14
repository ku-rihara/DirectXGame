#include "PrimitiveCylinder.h"

using namespace KetaEngine;
#include "3d/Mesh.h"
#include "Base/Dx/DirectXCommon.h"
#include <numbers>
#include <vector>

void PrimitiveCylinder::Init() {
    vertexNum_ = params_.divisions * params_.heightDivisions * kVerticesPerFace;
    IPrimitive::Init();
}

void PrimitiveCylinder::Rebuild() {
    vertexNum_ = params_.divisions * params_.heightDivisions * kVerticesPerFace;
    mesh_      = std::make_unique<Mesh>();
    mesh_->Init(DirectXCommon::GetInstance(), vertexNum_);
    Create();
}

void PrimitiveCylinder::Create() {
    const float startRad  = params_.startAngleDeg * (std::numbers::pi_v<float> / 180.0f);
    const float endRad    = params_.endAngleDeg   * (std::numbers::pi_v<float> / 180.0f);
    const float angleStep = (endRad - startRad) / float(params_.divisions);
    const float hStep     = 1.0f / float(params_.heightDivisions);


    uint32_t baseIdx = 0;
    for (int32_t j = 0; j < params_.heightDivisions; ++j) {
        float tTop = float(j)     * hStep;
        float tBot = float(j + 1) * hStep;
        float yTop = params_.height * (1.0f - tTop); 
        float yBot = params_.height * (1.0f - tBot);

        // 上下半径を縦位置でレム補間 (円錐台・楕円柱)
        float rxTop = params_.topRadiusX + (params_.bottomRadiusX - params_.topRadiusX) * tTop;
        float rzTop = params_.topRadiusZ + (params_.bottomRadiusZ - params_.topRadiusZ) * tTop;
        float rxBot = params_.topRadiusX + (params_.bottomRadiusX - params_.topRadiusX) * tBot;
        float rzBot = params_.topRadiusZ + (params_.bottomRadiusZ - params_.topRadiusZ) * tBot;

        // 上下の頂点カラーを縦位置で補間
        Vector4 colorTop = LerpColor(params_.topColor, params_.bottomColor, tTop);
        Vector4 colorBot = LerpColor(params_.topColor, params_.bottomColor, tBot);

        for (int32_t i = 0; i < params_.divisions; ++i) {
            float angle0 = startRad + float(i)     * angleStep;
            float angle1 = startRad + float(i + 1) * angleStep;
            float sin0   = std::sin(angle0), cos0 = std::cos(angle0);
            float sin1   = std::sin(angle1), cos1 = std::cos(angle1);
            float u0     = float(i)     / float(params_.divisions);
            float u1     = float(i + 1) / float(params_.divisions);

            // 三角形1: top0, top1, bot0
            mesh_->SetVertexPositionData(baseIdx + 0, Vector4(-sin0 * rxTop, yTop, cos0 * rzTop, 1.0f));
            mesh_->SetVertexTexcoordData(baseIdx + 0, GetUV(u0, tTop));
            mesh_->SetVertexNormData    (baseIdx + 0, Vector3(-sin0, 0.0f, cos0));
            mesh_->SetVertexColorData   (baseIdx + 0, colorTop);

            mesh_->SetVertexPositionData(baseIdx + 1, Vector4(-sin1 * rxTop, yTop, cos1 * rzTop, 1.0f));
            mesh_->SetVertexTexcoordData(baseIdx + 1, GetUV(u1, tTop));
            mesh_->SetVertexNormData    (baseIdx + 1, Vector3(-sin1, 0.0f, cos1));
            mesh_->SetVertexColorData   (baseIdx + 1, colorTop);

            mesh_->SetVertexPositionData(baseIdx + 2, Vector4(-sin0 * rxBot, yBot, cos0 * rzBot, 1.0f));
            mesh_->SetVertexTexcoordData(baseIdx + 2, GetUV(u0, tBot));
            mesh_->SetVertexNormData    (baseIdx + 2, Vector3(-sin0, 0.0f, cos0));
            mesh_->SetVertexColorData   (baseIdx + 2, colorBot);

            // 三角形2: bot0, top1, bot1
            mesh_->SetVertexPositionData(baseIdx + 3, Vector4(-sin0 * rxBot, yBot, cos0 * rzBot, 1.0f));
            mesh_->SetVertexTexcoordData(baseIdx + 3, GetUV(u0, tBot));
            mesh_->SetVertexNormData    (baseIdx + 3, Vector3(-sin0, 0.0f, cos0));
            mesh_->SetVertexColorData   (baseIdx + 3, colorBot);

            mesh_->SetVertexPositionData(baseIdx + 4, Vector4(-sin1 * rxTop, yTop, cos1 * rzTop, 1.0f));
            mesh_->SetVertexTexcoordData(baseIdx + 4, GetUV(u1, tTop));
            mesh_->SetVertexNormData    (baseIdx + 4, Vector3(-sin1, 0.0f, cos1));
            mesh_->SetVertexColorData   (baseIdx + 4, colorTop);

            mesh_->SetVertexPositionData(baseIdx + 5, Vector4(-sin1 * rxBot, yBot, cos1 * rzBot, 1.0f));
            mesh_->SetVertexTexcoordData(baseIdx + 5, GetUV(u1, tBot));
            mesh_->SetVertexNormData    (baseIdx + 5, Vector3(-sin1, 0.0f, cos1));
            mesh_->SetVertexColorData   (baseIdx + 5, colorBot);

            baseIdx += kVerticesPerFace;
        }
    }

    std::vector<uint32_t> indices(vertexNum_);
    for (uint32_t i = 0; i < vertexNum_; ++i) {
        indices[i] = i;
    }
    mesh_->SetIndexData(indices.data(), static_cast<uint32_t>(indices.size()));
}

Vector4 PrimitiveCylinder::LerpColor(const Vector4& a, const Vector4& b, float t) {
    return Vector4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t);
}

Vector2 PrimitiveCylinder::GetUV(float u, float v) const {
    switch (params_.uvModeInt) {
    case 1:  return Vector2(u, 1.0f - v);   // 下→上フリップ
    case 2:  return Vector2(v, u);          // 横方向
    default: return Vector2(u, v);          // 上→下
    }
}

void PrimitiveCylinder::SetTexture(const std::string& name) {
    IPrimitive::SetTexture(name);
}

void PrimitiveCylinder::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    IPrimitive::Draw(worldTransform, viewProjection, textureHandle);
}
