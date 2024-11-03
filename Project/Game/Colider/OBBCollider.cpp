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
    object3d_.reset(Object3d::CreateModel("CollisionCube", ".obj"));
    transform_.Init();
    obb_.size = { 1.5f,1.5f,7.5f };
}

void OBBCollider::UpdateWorldTransform() {
 
    // OBBの中心をワールド座標に更新
    transform_.translation_ = GetBaseCenterPosition();
    transform_.rotation_ = GetRotate();
    transform_.scale_ = obb_.size;

     obb_.center = GetBaseCenterPosition();

    // 回転ベクトルを更新（各軸に対応する方向ベクトル）
    obb_.orientations[0] = transform_.GetRightVector();    // X軸方向ベクトル
    obb_.orientations[1] = transform_.GetUpVector();       // Y軸方向ベクトル
    obb_.orientations[2] = transform_.GetForwardVector();  // Z軸方向ベクトル
  
    // ワールド変換行列の更新
    transform_.UpdateMatrix();
}

void OBBCollider::DrawDebugCube(const ViewProjection& viewProjection) {
    BaseCollider::DrawDebugCube(viewProjection);
}

Vector3 OBBCollider::GetBaseCenterPosition() const {
    return BaseCollider::GetBaseCenterPosition();
}

Vector3 OBBCollider::GetRotate() const {
    Vector3 rotate;
    rotate = transform_.rotation_;
    return rotate;
}