#include "Collider.h"
#include"Colider/CollisionManager.h"

Collider::Collider() {
	// 自動登録する
	CollisionManager::AddCollider(this);
}

Collider::~Collider() {
	// 自動登録する
	CollisionManager::RemoveCollider(this);
}


void Collider::Init() { 
	//モデル作成
	object3d_.reset(Object3d::CreateModel("CollisionCube", ".obj"));
	transform_.Init();
}

void Collider::UpdateWorldTransform() {
	//ワールド座標をワールドトランスフォームに適用
	transform_.translation_ =GetBaseCenterPosition();

	aabb_.min = transform_.translation_ - radiusVector_;
	aabb_.max = transform_.translation_ + radiusVector_;

	transform_.scale_ = radiusVector_;

	transform_.UpdateMatrix();
}

void Collider::Draw(const ViewProjection& viewProjection) {
	//モデルの描画
	object3d_->Draw(transform_,viewProjection);
}