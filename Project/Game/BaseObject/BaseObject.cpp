#include "BaseObject.h"

void BaseObject::Init() {
    baseTransform_.Init();
}

void BaseObject::Update() {
    baseTransform_.UpdateMatrix();
   /* obj3d_->transform_.UpdateMatrix();*/
}

Vector3 BaseObject::GetWorldPosition() const {
    Vector3 worldPos;
    // ワールド行列の平行移動成分を取得
    worldPos.x = baseTransform_.matWorld_.m[3][0];
    worldPos.y = baseTransform_.matWorld_.m[3][1];
    worldPos.z = baseTransform_.matWorld_.m[3][2];

    return worldPos;
}

void BaseObject::SetColor(const Vector4& color) {
    obj3d_->objColor_.SetColor(color);
}