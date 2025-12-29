#include "AABBCollider.h"

using namespace KetaEngine;
#include"Collider/CollisionManager.h"

AABBCollider::AABBCollider() {
    // 自動登録する
    CollisionManager::AddCollider(this);
}

AABBCollider::~AABBCollider() {
    // 自動登録する
    CollisionManager::RemoveCollider(this);
}

void AABBCollider::Init() {
   
    debugLine_.reset(Line3D::Create(24));
    cTransform_.Init();
}

void AABBCollider::UpdateWorldTransform() {
    // AABBのワールド行列を更新
    cTransform_.translation_ = GetCollisionPos();
    aabb_.min = cTransform_.translation_ - collisionScale_;
    aabb_.max = cTransform_.translation_ + collisionScale_;
    cTransform_.scale_ = collisionScale_;
    cTransform_.UpdateMatrix();
}

void AABBCollider::SetDebugCube() {
    debugLine_->SetCubeWireframe(cTransform_.GetWorldPos(), cTransform_.scale_, lineColor_);
}

Vector3 AABBCollider::GetCollisionPos() const {
   return BaseCollider::GetCollisionPos();
}
