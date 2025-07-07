#include "AABBCollider.h"
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
   
   debugLine_.Init(24);
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

void AABBCollider::DrawDebugCube(const ViewProjection& viewProjection) {
    debugLine_.DrawCubeWireframe(cTransform_.GetWorldPos(), cTransform_.scale_, lineColor_);
    debugLine_.Draw(viewProjection);
}

Vector3 AABBCollider::GetCollisionPos() const {
   return BaseCollider::GetCollisionPos();
}
