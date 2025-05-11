#include "PrimitiveBox.h"
#include "3d/Mesh.h"
#include "base/SkyBoxRenderer.h"
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

    //// 右面、描画indexは[0,1,2][2,1,3]で内側を向く
    //mesh_ -> SetVertexNormData(0, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(1, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(2, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(3, Vector3(1.0f, 1.0f, 1.0f));
    //// 左面-、描画indexは[4,5,6][6,5,7]
    //mesh_ -> SetVertexNormData(4, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(5, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(6, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(7, Vector3(1.0f, 1.0f, 1.0f));
    //// 前面-、描画indexは[8,9,10][10,9,11]
    //mesh_ -> SetVertexNormData(8,  Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(9,  Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(10, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(11, Vector3(1.0f, 1.0f, 1.0f));
    //// 背面-（奥側）、描画indexは[12,13,14][34,13,15]
    //mesh_ -> SetVertexNormData(12, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(13, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(14, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(15, Vector3(1.0f, 1.0f, 1.0f));
    //// 上面-、描画indexは[16,17,18][18,17,19]
    //mesh_ -> SetVertexNormData(16, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(17, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(18, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(19, Vector3(1.0f, 1.0f, 1.0f));
    //// 下面-、描画indexは[20,21,22][22,21,23]
    //mesh_ -> SetVertexNormData(20, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(21, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(22, Vector3(1.0f, 1.0f, 1.0f));
    //mesh_ -> SetVertexNormData(23, Vector3(1.0f, 1.0f, 1.0f));


    // // 右面、描画indexは[0,1,2][2,1,3]で内側を向く
    //mesh_->SetVertexTexcoordData(0, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(1, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(2, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(3, Vector2(1.0f, 1.0f));
    //// 左面-、描画indexは[4,5,6][6,5,7]
    //mesh_->SetVertexTexcoordData(4, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(5, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(6, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(7, Vector2(1.0f, 1.0f));
    //// 前面-、描画indexは[8,9,10][10,9,11]
    //mesh_->SetVertexTexcoordData(8,  Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(9,  Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(10, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(11, Vector2(1.0f, 1.0f));
    //// 背面-（奥側）、描画indexは[12,13,14][34,13,15]
    //mesh_->SetVertexTexcoordData(12, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(13, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(14, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(15, Vector2(1.0f, 1.0f));
    //// 上面-、描画indexは[16,17,18][18,17,19]
    //mesh_->SetVertexTexcoordData(16, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(17, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(18, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(19, Vector2(1.0f, 1.0f));
    //// 下面-、描画indexは[20,21,22][22,21,23]
    //mesh_->SetVertexTexcoordData(20, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(21, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(22, Vector2(1.0f, 1.0f));
    //mesh_->SetVertexTexcoordData(23, Vector2(1.0f, 1.0f));
}

void PrimitiveBox::SetTexture(const std::string& name) {
    IPrimitive::SetTexture(name);
}

void PrimitiveBox::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    /*SkyBoxRenderer::GetInstance()->SetPiplelineState();*/
    IPrimitive::Draw(worldTransform, viewProjection, textureHandle);
}