#include "BaseAABBCollisionBox.h"
#include <imgui.h>

void BaseAABBCollisionBox::Init() {
    transform_.Init();
    /*SetZeroSizeCollision();*/
}
void BaseAABBCollisionBox::Update(const float& timeSpeed) {
    timeSpeed;

     SetCollisionScale(size_);
    SetIsAdaptCollision(isCollision_);
    transform_.UpdateMatrix();
}


Vector3 BaseAABBCollisionBox::GetCollisionPos() const {

    // ワールド座標に変換
    Vector3 worldPos = TransformMatrix(offset_, transform_.matWorld_);
    return worldPos;
}


void BaseAABBCollisionBox::SetParentTransform(WorldTransform* transform) {
    transform_.parent_ = transform;
}

void BaseAABBCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
}

void BaseAABBCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
}

