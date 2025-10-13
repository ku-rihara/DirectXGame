#include "PrimitiveBox.h"
#include "3d/Mesh.h"
#include <numbers>

void PrimitiveBox::Init() {
    vertexNum_ = 24;

    IPrimitive::Init();
}

void PrimitiveBox::Create() {

    // 右面、描画indexは[0,1,2][2,1,3]で内側を向く
    mesh_->SetVertexPositionData(0, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mesh_->SetVertexPositionData(1, Vector4(1.0f, 1.0f, -1.0f, 1.0f));
    mesh_->SetVertexPositionData(2, Vector4(1.0f, -1.0f, 1.0f, 1.0f));
    mesh_->SetVertexPositionData(3, Vector4(1.0f, -1.0f, -1.0f, 1.0f));
    // 左面、描画indexは[4,5,6][6,5,7]
    mesh_->SetVertexPositionData(4, Vector4(-1.0f, 1.0f, -1.0f, 1.0f));
    mesh_->SetVertexPositionData(5, Vector4(-1.0f, 1.0f, 1.0f, 1.0f));
    mesh_->SetVertexPositionData(6, Vector4(-1.0f, -1.0f, -1.0f, 1.0f));
    mesh_->SetVertexPositionData(7, Vector4(-1.0f, -1.0f, 1.0f, 1.0f));
    // 前面、描画indexは[8,9,10][10,9,11]
    mesh_->SetVertexPositionData(8, Vector4(-1.0f, 1.0f, 1.0f, 1.0f));
    mesh_->SetVertexPositionData(9, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mesh_->SetVertexPositionData(10, Vector4(-1.0f, -1.0f, 1.0f, 1.0f));
    mesh_->SetVertexPositionData(11, Vector4(1.0f, -1.0f, 1.0f, 1.0f));
    // 背面（奥側）、描画indexは[12,13,14][14,13,15]
    mesh_->SetVertexPositionData(12, Vector4(1.0f, 1.0f, -1.0f, 1.0f));
    mesh_->SetVertexPositionData(13, Vector4(-1.0f, 1.0f, -1.0f, 1.0f));
    mesh_->SetVertexPositionData(14, Vector4(1.0f, -1.0f, -1.0f, 1.0f));
    mesh_->SetVertexPositionData(15, Vector4(-1.0f, -1.0f, -1.0f, 1.0f));
    // 上面、描画indexは[16,17,18][18,17,19]
    mesh_->SetVertexPositionData(16, Vector4(-1.0f, 1.0f, -1.0f, 1.0f));
    mesh_->SetVertexPositionData(17, Vector4(1.0f, 1.0f, -1.0f, 1.0f));
    mesh_->SetVertexPositionData(18, Vector4(-1.0f, 1.0f, 1.0f, 1.0f));
    mesh_->SetVertexPositionData(19, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    // 下面、描画indexは[20,21,22][22,21,23]
    mesh_->SetVertexPositionData(20, Vector4(-1.0f, -1.0f, 1.0f, 1.0f));
    mesh_->SetVertexPositionData(21, Vector4(1.0f, -1.0f, 1.0f, 1.0f));
    mesh_->SetVertexPositionData(22, Vector4(-1.0f, -1.0f, -1.0f, 1.0f));
    mesh_->SetVertexPositionData(23, Vector4(1.0f, -1.0f, -1.0f, 1.0f));

    // 各面の三角形インデックス（内向き）
    uint32_t indices[] = {
        // 右面
        0, 1, 2, 2, 1, 3,
        // 左面
        4, 5, 6, 6, 5, 7,
        // 前面
        8, 9, 10, 10, 9, 11,
        // 背面
        12, 13, 14, 14, 13, 15,
        // 上面
        16, 17, 18, 18, 17, 19,
        // 下面
        20, 21, 22, 22, 21, 23};

    // インデックスをセット
    mesh_->SetIndexData(indices, static_cast<uint32_t>(std::size(indices)));
}

void PrimitiveBox::SetTexture(const std::string& name) {
    IPrimitive::SetTexture(name);
}

void PrimitiveBox::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    IPrimitive::Draw(worldTransform, viewProjection, textureHandle);
}