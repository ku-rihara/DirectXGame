#include "PrimitiveCylinder.h"

using namespace KetaEngine;
#include "3d/Mesh.h"
#include "Base/Dx/DirectXCommon.h"
#include "MathFunction.h"
#include <numbers>
#include <vector>

void PrimitiveCylinder::Init() {
    vertexNum_ = params_.divisions * params_.heightDivisions * kVerticesPerFace;
    IPrimitive::Init();
}

void PrimitiveCylinder::Rebuild() {
    RebuildInternal(params_.divisions * params_.heightDivisions * kVerticesPerFace);
    Create();
}


/// 軸に応じて (円周X, 円周Y, 高さ方向) を座標に変換する
Vector4 PrimitiveCylinder::ApplyAxis(float circX, float circY, float h) const {
    switch (params_.axisInt) {
    case 1: // X軸中心: 高さ=X、円周=YZ
        return Vector4(h, circX, circY, 1.0f);
    case 2: // Z軸中心: 高さ=Z、円周=XY
        return Vector4(circX, circY, h, 1.0f);
    default: // Y軸中心 (デフォルト): 高さ=Y、円周=XZ
        return Vector4(circX, h, circY, 1.0f);
    }
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

        float rxTop = params_.topRadiusX + (params_.bottomRadiusX - params_.topRadiusX) * tTop;
        float rzTop = params_.topRadiusZ + (params_.bottomRadiusZ - params_.topRadiusZ) * tTop;
        float rxBot = params_.topRadiusX + (params_.bottomRadiusX - params_.topRadiusX) * tBot;
        float rzBot = params_.topRadiusZ + (params_.bottomRadiusZ - params_.topRadiusZ) * tBot;

        Vector4 colorTop = Lerp(params_.topColor, params_.bottomColor, tTop);
        Vector4 colorBot = Lerp(params_.topColor, params_.bottomColor, tBot);

        for (int32_t i = 0; i < params_.divisions; ++i) {
            float angle0 = startRad + float(i)     * angleStep;
            float angle1 = startRad + float(i + 1) * angleStep;
            float sin0   = std::sin(angle0), cos0 = std::cos(angle0);
            float sin1   = std::sin(angle1), cos1 = std::cos(angle1);
            float u0     = float(i)     / float(params_.divisions);
            float u1     = float(i + 1) / float(params_.divisions);

            // 三角形1: top0, top1, bot0
            mesh_->SetVertexPositionData(baseIdx + 0, ApplyAxis(-sin0 * rxTop, cos0 * rzTop, yTop));
            mesh_->SetVertexTexcoordData(baseIdx + 0, GetUV(u0, tTop));
            mesh_->SetVertexNormData    (baseIdx + 0, Vector3(-sin0, 0.0f, cos0));
            mesh_->SetVertexColorData   (baseIdx + 0, colorTop);

            mesh_->SetVertexPositionData(baseIdx + 1, ApplyAxis(-sin1 * rxTop, cos1 * rzTop, yTop));
            mesh_->SetVertexTexcoordData(baseIdx + 1, GetUV(u1, tTop));
            mesh_->SetVertexNormData    (baseIdx + 1, Vector3(-sin1, 0.0f, cos1));
            mesh_->SetVertexColorData   (baseIdx + 1, colorTop);

            mesh_->SetVertexPositionData(baseIdx + 2, ApplyAxis(-sin0 * rxBot, cos0 * rzBot, yBot));
            mesh_->SetVertexTexcoordData(baseIdx + 2, GetUV(u0, tBot));
            mesh_->SetVertexNormData    (baseIdx + 2, Vector3(-sin0, 0.0f, cos0));
            mesh_->SetVertexColorData   (baseIdx + 2, colorBot);

            // 三角形2: bot0, top1, bot1
            mesh_->SetVertexPositionData(baseIdx + 3, ApplyAxis(-sin0 * rxBot, cos0 * rzBot, yBot));
            mesh_->SetVertexTexcoordData(baseIdx + 3, GetUV(u0, tBot));
            mesh_->SetVertexNormData    (baseIdx + 3, Vector3(-sin0, 0.0f, cos0));
            mesh_->SetVertexColorData   (baseIdx + 3, colorBot);

            mesh_->SetVertexPositionData(baseIdx + 4, ApplyAxis(-sin1 * rxTop, cos1 * rzTop, yTop));
            mesh_->SetVertexTexcoordData(baseIdx + 4, GetUV(u1, tTop));
            mesh_->SetVertexNormData    (baseIdx + 4, Vector3(-sin1, 0.0f, cos1));
            mesh_->SetVertexColorData   (baseIdx + 4, colorTop);

            mesh_->SetVertexPositionData(baseIdx + 5, ApplyAxis(-sin1 * rxBot, cos1 * rzBot, yBot));
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


Vector2 PrimitiveCylinder::GetUV(float u, float v) const {
    switch (params_.uvModeInt) {
    case 1:  return Vector2(u, 1.0f - v);
    case 2:  return Vector2(v, u);
    default: return Vector2(u, v);
    }
}

