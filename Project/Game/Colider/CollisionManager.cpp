#include "CollisionManager.h"
#include "AABB.h"

// 静的メンバ変数の初期化
std::list<Collider*> CollisionManager::colliders_;

void CollisionManager::Init() { 


	//JSON関連****************************************************************
	globalParameter_ = GlobalParameter::GetInstance();
	const char* groupName = "CollisionManager";
	// グループを追加
	globalParameter_->CreateGroup(groupName);
	globalParameter_->AddItem(groupName, "isColliderVisible", isColliderVisible_);
}

void CollisionManager::AddCollider(Collider* collider) {
	collider->Init();
	colliders_.push_back(collider);
}

void CollisionManager::RemoveCollider(Collider* collider) {
	colliders_.remove(collider);  // リストから削除
}
void CollisionManager::Reset() {
	// リストを空っぽにする
	colliders_.clear();
}

void CollisionManager::Update() { 
	//imguiからパラメータを取得
	ApplyGlobalParameter();

	UpdateWorldTransform();

	CheckAllCollisions();
}

void CollisionManager::UpdateWorldTransform() {
	
	//非表示なら抜ける
	if (!isColliderVisible_) {
		return;
	}
	//全てのコライダーについて行列更新をする
	for (Collider* colider : colliders_) {
		colider->UpdateWorldTransform();
	}

	
}

void CollisionManager::Draw(const ViewProjection& viewProjection) {
	// 非表示なら抜ける
	if (!isColliderVisible_) {
		return;
	}
	//全てのコライダーを描画する
	for (Collider* colider : colliders_) {
		colider->Draw(viewProjection);
	}
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	AABB aabbA = colliderA->GetAABB();
	AABB aabbB = colliderB->GetAABB();

	if (IsCollision(aabbA, aabbB)) {
		if (!colliderA->GetIsColliding() || !colliderB->GetIsColliding()) {
			colliderA->OnCollisionEnter(colliderB);
			colliderB->OnCollisionEnter(colliderA);
		}
		else {
			colliderA->OnCollisionStay(colliderB);
			colliderB->OnCollisionStay(colliderA);
		}

		colliderA->SetColliding(true);
		colliderB->SetColliding(true);
	}
	else {
		if (colliderA->GetIsColliding()) {
			colliderA->OnCollisionExit(colliderB);
			colliderA->SetColliding(false);
		}
		if (colliderB->GetIsColliding()) {
			colliderB->OnCollisionExit(colliderA);
			colliderB->SetColliding(false);
		}
	}
}

void CollisionManager::CheckAllCollisions() {
	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		// イテレータAからコライダーAを取得する
		Collider* colliderA = *itrA;//ダブルポインタから中身のポインタを取り出す処理

		// イテレーターBはイテレータAの次の要素から回す（重複判定を回避）
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;//ダブルポインタから中身のポインタを取り出す処理

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
