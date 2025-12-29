#include "BaseAABBCollisionBox.h"
#include <imgui.h>

void BaseAABBCollisionBox::Init() {
    transform_.Init();
    /*SetZeroSizeCollision();*/
}
void BaseAABBCollisionBox::Update() {

    SetCollisionScale(size_);
    SetIsAdaptCollision(isAbleCollision_);
    transform_.UpdateMatrix();
}

Vector3 BaseAABBCollisionBox::GetCollisionPos() const {

    // ワールド座標に変換
    Vector3 worldPos = TransformMatrix(offset_, transform_.matWorld_);
    return worldPos;
}

void BaseAABBCollisionBox::SetParentTransform(KetaEngine::WorldTransform* transform) {
    transform_.parent_ = transform;
}

void BaseAABBCollisionBox::OnCollisionEnter([[maybe_unused]] KetaEngine::BaseCollider* other) {
}

void BaseAABBCollisionBox::OnCollisionStay([[maybe_unused]] KetaEngine::BaseCollider* other) {
}
