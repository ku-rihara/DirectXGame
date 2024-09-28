#include "CollisionManager.h"

void CollisionManager::Init() { 
	model_.reset(Model::Create("CollisionSphere"));

	globalParameter_ = GlobalParameter::GetInstance();
	const char* groupName = "CollisionManager";
	// グループを追加
	globalParameter_->CreateGroup(groupName);
	globalParameter_->AddItem(groupName, "isColliderVisible", isColliderVisible_);
}

void CollisionManager::AddCollider(Colider* collider) {
	// コライダーをリストに登録
	colliders_.push_back(collider);
}

void CollisionManager::Reset() {
	// リストを空っぽにする
	colliders_.clear();
}

void CollisionManager::Update() { 
	ApplyGlobalParameter();
}

void CollisionManager::UpdateWorldTransform() {
	
	//非表示なら抜ける
	if (!isColliderVisible_) {
		return;
	}
	//全てのコライダーについて
	for (Colider* colider : colliders_) {
		colider->UpdateWorldTransform();
	}
}

void CollisionManager::Draw(const ViewProjection& viewProjection) {
	// 非表示なら抜ける
	if (!isColliderVisible_) {
		return;
	}
	//全てのコライダー
	for (Colider* colider : colliders_) {
		colider->Draw(model_.get(), viewProjection);
	}
}

void CollisionManager::CheckCollisionPair(Colider* colliderA, Colider* colliderB) {
	//コライダーの座標取得
	Vector3 posA = colliderA->GetBaseCenterPosition();
	Vector3 posB = colliderB->GetBaseCenterPosition();
	//座標の差分ベクトル
	Vector3 subtract = posB - posA;
	//座標AとBの距離を求める
	float distance = Length(subtract);
	//球同士の交差判定
	if (distance <= colliderA->GetRadius()+colliderB->GetRadius()) {
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);
	
	}
}

void CollisionManager::CheckAllCollisions() {
	// リスト内のペアを総当たり
	std::list<Colider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		// イテレータAからコライダーAを取得する
		Colider* colliderA = *itrA;//ダブルポインタから中身のポインタを取り出す処理

		// イテレーターBはイテレータAの次の要素から回す（重複判定を回避）
		std::list<Colider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Colider* colliderB = *itrB;//ダブルポインタから中身のポインタを取り出す処理

			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::ApplyGlobalParameter() {
	
	GlobalParameter* globalParameter = GlobalParameter::GetInstance();
	const char* groupName = "CollisionManager";

	isColliderVisible_ = globalParameter->GetValue<bool>(groupName, "isColliderVisible");
}