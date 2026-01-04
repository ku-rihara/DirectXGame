#include "PrimitiveSphere.h"

using namespace KetaEngine;
#include "3d/Mesh.h"
#include <numbers>
#include <vector>

void PrimitiveSphere::Init() {
    // 緯度分割数と経度分割数
    const uint32_t kLatDivision = 16; // 緯度方向の分割数
    const uint32_t kLonDivision = 32; // 経度方向の分割数

    // 頂点数
    vertexNum_ = (kLatDivision + 1) * (kLonDivision + 1);

    IPrimitive::Init();
}

void PrimitiveSphere::Create() {
    const float kRadius         = 1.0f;
    const uint32_t kLatDivision = 16; // 緯度方向の分割数
    const uint32_t kLonDivision = 32; // 経度方向の分割数

    const float kLatStep = std::numbers::pi_v<float> / float(kLatDivision);
    const float kLonStep = 2.0f * std::numbers::pi_v<float> / float(kLonDivision);

    // 頂点データの生成
    uint32_t vertexIndex = 0;
    for (uint32_t lat = 0; lat <= kLatDivision; ++lat) {
        float theta    = lat * kLatStep; // 緯度角度
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (uint32_t lon = 0; lon <= kLonDivision; ++lon) {
            float phi    = lon * kLonStep; // 経度角度
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            // 球面座標から直交座標への変換
            Vector3 position(
                kRadius * sinTheta * cosPhi,
                kRadius * cosTheta,
                kRadius * sinTheta * sinPhi);

            // 法線
            Vector3 normal(
                sinTheta * cosPhi,
                cosTheta,
                sinTheta * sinPhi);

            // テクスチャ座標
            Vector2 texcoord(
                float(lon) / float(kLonDivision),
                float(lat) / float(kLatDivision));

            mesh_->SetVertexPositionData(vertexIndex, Vector4(position.x, position.y, position.z, 1.0f));
            mesh_->SetVertexNormData(vertexIndex, normal);
            mesh_->SetVertexTexcoordData(vertexIndex, texcoord);

            ++vertexIndex;
        }
    }

    // インデックスデータの生成
    std::vector<uint32_t> indices;
    for (uint32_t lat = 0; lat < kLatDivision; ++lat) {
        for (uint32_t lon = 0; lon < kLonDivision; ++lon) {
            uint32_t current = lat * (kLonDivision + 1) + lon;
            uint32_t next    = current + kLonDivision + 1;

            // 三角形1
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // 三角形2
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    mesh_->SetIndexData(indices.data(), static_cast<uint32_t>(indices.size()));
}

void PrimitiveSphere::SetTexture(const std::string& name) {
    IPrimitive::SetTexture(name);
}

void PrimitiveSphere::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    IPrimitive::Draw(worldTransform, viewProjection, textureHandle);
}