#include "PrimitivePlane.h"
#include"3d/Mesh.h"


void PrimitivePlane::Init() {
    vertexNum_ = 4;
    IPrimitive::Init();
}

void PrimitivePlane::Create() {
    mesh_->SetVertexPositionData(0, Vector4(-0.5f, -0.5f, 0.0f, 1.0f)); // 左下
    mesh_->SetVertexTexcoordData(0, Vector2(0.0f, 1.0f));
    mesh_->SetVertexNormData(0, Vector3(0.0f, 0.0f, 1.0f));

    mesh_->SetVertexPositionData(1, Vector4(0.5f, -0.5f, 0.0f, 1.0f)); // 右下
    mesh_->SetVertexTexcoordData(1, Vector2(1.0f, 1.0f));
    mesh_->SetVertexNormData(1, Vector3(0.0f, 0.0f, 1.0f));

    mesh_->SetVertexPositionData(2, Vector4(-0.5f, 0.5f, 0.0f, 1.0f)); // 左上
    mesh_->SetVertexTexcoordData(2, Vector2(0.0f, 0.0f));
    mesh_->SetVertexNormData(2, Vector3(0.0f, 0.0f, 1.0f));

    mesh_->SetVertexPositionData(3, Vector4(0.5f, 0.5f, 0.0f, 1.0f)); // 右上
    mesh_->SetVertexTexcoordData(3, Vector2(1.0f, 0.0f));
    mesh_->SetVertexNormData(3, Vector3(0.0f, 0.0f, 1.0f));
}


