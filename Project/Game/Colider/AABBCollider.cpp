#include "AABBCollider.h"
#include "base/TextureManager.h"
#include "assert.h"

#include"Colider/CollisionManager.h"

AABBCollider::AABBCollider() {
    // 自動登録する
    CollisionManager::AddAABBCollider(this);
}

AABBCollider::~AABBCollider() {
    // 自動登録する
    CollisionManager::RemoveAABBCollider(this);
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

void AABBCollider::Draw(const ViewProjection& viewProjection) {
    // AABBのデバッグ表示
    if (object3d_) {
        object3d_->Draw(transform_, viewProjection);
    }
}

Vector3 AABBCollider::GetBaseCenterPosition() const {
   return BaseCollider::GetBaseCenterPosition();
}
