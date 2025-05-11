#include "PrimitivePlane.h"
#include "3d/Mesh.h"

void PrimitivePlane::Init() {
    vertexNum_ = 6;
    IPrimitive::Init();
}

void PrimitivePlane::Create() {
    // 頂点データの設定
    mesh_->SetVertexPositionData(0, Vector4(-1.0f, -1.0f, 0.0f, 1.0f)); // 左下
    mesh_->SetVertexTexcoordData(0, Vector2(0.0f, 1.0f));
    mesh_->SetVertexNormData(0, Vector3(0.0f, 0.0f, -1.0f));

    mesh_->SetVertexPositionData(1, Vector4(-1.0f, 1.0f, 0.0f, 1.0f)); // 右下
    mesh_->SetVertexTexcoordData(1, Vector2(0.0f, 0.0f));
    mesh_->SetVertexNormData(1, Vector3(0.0f, 0.0f, -1.0f));

    mesh_->SetVertexPositionData(2, Vector4(1.0f, -1.0f, 0.0f, 1.0f)); // 左上
    mesh_->SetVertexTexcoordData(2, Vector2(1.0f, 1.0f));
    mesh_->SetVertexNormData(2, Vector3(0.0f, 0.0f, -1.0f));

    mesh_->SetVertexPositionData(3, Vector4(-1.0f, 1.0f, 0.0f, 1.0f)); // 右上
    mesh_->SetVertexTexcoordData(3, Vector2(0.0f, 0.0f));
    mesh_->SetVertexNormData(3, Vector3(0.0f, 0.0f, -1.0f));

    mesh_->SetVertexPositionData(4, Vector4(1.0f, 1.0f, 0.0f, 1.0f)); // 右上
    mesh_->SetVertexTexcoordData(4, Vector2(1.0f, 0.0f));
    mesh_->SetVertexNormData(4, Vector3(0.0f, 0.0f, -1.0f));

    mesh_->SetVertexPositionData(5, Vector4(1.0f, -1.0f, 0.0f, 1.0f)); // 右上
    mesh_->SetVertexTexcoordData(5, Vector2(1.0f, 1.0f));
    mesh_->SetVertexNormData(5, Vector3(0.0f, 0.0f, -1.0f));

   
}


void PrimitivePlane::SetTexture(const std::string& name) {
    IPrimitive::SetTexture(name);
}

void PrimitivePlane::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    IPrimitive::Draw(worldTransform, viewProjection, textureHandle);
}