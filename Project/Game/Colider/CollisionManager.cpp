#include "CollisionManager.h"
#include "Box.h"

// 静的メンバ変数の初期化
std::list<AABBCollider*> CollisionManager::AABBColliders_;

void CollisionManager::Init() { 


	//JSON関連****************************************************************
	globalParameter_ = GlobalParameter::GetInstance();
	const char* groupName = "CollisionManager";
	// グループを追加
	globalParameter_->CreateGroup(groupName);
	globalParameter_->AddItem(groupName, "isColliderVisible", isColliderVisible_);
}

void CollisionManager::AddAABBCollider(AABBCollider* collider) {
	collider->Init();
	AABBColliders_.push_back(collider);
}

void CollisionManager::RemoveAABBCollider(AABBCollider* collider) {
	AABBColliders_.remove(collider);  // リストから削除
}
void CollisionManager::Reset() {
	// リストを空っぽにする
	AABBColliders_.clear();
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
	for (AABBCollider* colider : AABBColliders_) {
		colider->UpdateWorldTransform();
	}

	
}

void CollisionManager::Draw(const ViewProjection& viewProjection) {
	// 非表示なら抜ける
	if (!isColliderVisible_) {
		return;
	}
	//全てのコライダーを描画する
	for (AABBCollider* AABBCollider : AABBColliders_) {
		AABBCollider->Draw(viewProjection);
	}
}

void CollisionManager::CheckCollisionPair(AABBCollider* colliderA, AABBCollider* colliderB) {
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
	std::list<AABBCollider*>::iterator itrA = AABBColliders_.begin();
	for (; itrA != AABBColliders_.end(); ++itrA) {
		// イテレータAからコライダーAを取得する
		AABBCollider* colliderA = *itrA;//ダブルポインタから中身のポインタを取り出す処理

		// イテレーターBはイテレータAの次の要素から回す（重複判定を回避）
		std::list<AABBCollider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != AABBColliders_.end(); ++itrB) {
			AABBCollider* colliderB = *itrB;//ダブルポインタから中身のポインタを取り出す処理

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
