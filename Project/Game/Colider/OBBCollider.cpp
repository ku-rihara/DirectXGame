#include "OBBCollider.h"
#include"Colider/CollisionManager.h"

OBBCollider::OBBCollider() {
    // 自動登録する
    CollisionManager::AddCollider(this);
}

OBBCollider::~OBBCollider() {
    // 自動登録する
    CollisionManager::RemoveCollider(this);
}


void OBBCollider::Init() {
    cObject3d_.reset(Object3d::CreateModel("CollisionCube", ".obj"));
    cTransform_.Init();
    obb_.size = { 1.0f,1.0f,1.0f };
}

void OBBCollider::UpdateWorldTransform() {
 
    // OBBの中心をワールド座標に更新
    cTransform_.translation_ = GetBaseCenterPosition();
    cTransform_.rotation_ = GetRotate();
    cTransform_.scale_ = obb_.size;

     obb_.center = GetBaseCenterPosition();

    // 回転ベクトルを更新（各軸に対応する方向ベクトル）
    obb_.orientations[0] = cTransform_.GetRightVector();    // X軸方向ベクトル
    obb_.orientations[1] = cTransform_.GetUpVector();       // Y軸方向ベクトル
    obb_.orientations[2] = cTransform_.GetForwardVector();  // Z軸方向ベクトル
  
    // ワールド変換行列の更新
    cTransform_.UpdateMatrix();
}

void OBBCollider::DrawDebugCube(const ViewProjection& viewProjection) {
    BaseCollider::DrawDebugCube(viewProjection);
}

Vector3 OBBCollider::GetBaseCenterPosition() const {
    return BaseCollider::GetBaseCenterPosition();
}

Vector3 OBBCollider::GetRotate() const {
    Vector3 rotate;
    rotate = cTransform_.rotation_;
    return rotate;
}