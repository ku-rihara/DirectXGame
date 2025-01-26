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
    // モデル作成（デバッグ用の可視化）
    cObject3d_.reset(Object3d::CreateModel("DebugCube", ".obj"));
    cTransform_.Init();
}

void AABBCollider::UpdateWorldTransform() {
    // AABBのワールド行列を更新
    cTransform_.translation_ = GetCollisionPos();
    aabb_.min = cTransform_.translation_ - radiusVector_;
    aabb_.max = cTransform_.translation_ + radiusVector_;
    cTransform_.scale_ = radiusVector_;
    cTransform_.UpdateMatrix();
}

void AABBCollider::DrawDebugCube(const ViewProjection& viewProjection) {
    BaseCollider::DrawDebugCube(viewProjection);
}

Vector3 AABBCollider::GetCollisionPos() const {
   return BaseCollider::GetCollisionPos();
}
