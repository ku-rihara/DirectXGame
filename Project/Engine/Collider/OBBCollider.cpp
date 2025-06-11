#include "OBBCollider.h"
#include"Collider/CollisionManager.h"

OBBCollider::OBBCollider() {
    // 自動登録する
    CollisionManager::AddCollider(this);
}

OBBCollider::~OBBCollider() {
    // 自動登録する
    CollisionManager::RemoveCollider(this);
}


void OBBCollider::Init() {
    cObject3d_.reset(Object3d::CreateModel("DebugCube.obj"));
    cTransform_.Init();
    obb_.size = { 1.5f,1.5f,1.5f };
}

void OBBCollider::UpdateWorldTransform() {
 
    // OBBの中心をワールド座標に更新
    cTransform_.translation_ = GetCollisionPos();
    cTransform_.rotation_ = GetRotate();
    cTransform_.scale_ = obb_.size;

     obb_.center = GetCollisionPos();

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

Vector3 OBBCollider::GetCollisionPos() const {
    return BaseCollider::GetCollisionPos();
}

Vector3 OBBCollider::GetRotate() const {
    Vector3 rotate;
    rotate = cTransform_.rotation_;
    return rotate;
}