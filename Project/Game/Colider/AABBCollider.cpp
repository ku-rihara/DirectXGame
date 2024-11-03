#include "AABBCollider.h"
#include"Colider/CollisionManager.h"

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
    object3d_.reset(Object3d::CreateModel("CollisionCube", ".obj"));
    transform_.Init();
}

void AABBCollider::UpdateWorldTransform() {
    // AABBのワールド行列を更新
    transform_.translation_ = GetBaseCenterPosition();
    aabb_.min = transform_.translation_ - radiusVector_;
    aabb_.max = transform_.translation_ + radiusVector_;
    transform_.scale_ = radiusVector_;
    transform_.UpdateMatrix();
}

void AABBCollider::DrawDebugCube(const ViewProjection& viewProjection) {
    BaseCollider::DrawDebugCube(viewProjection);
}

Vector3 AABBCollider::GetBaseCenterPosition() const {
   return BaseCollider::GetBaseCenterPosition();
}
