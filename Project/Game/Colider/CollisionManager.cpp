#include "CollisionManager.h"
#include "Box.h"

// 静的メンバ変数の初期化
std::list<BaseCollider*> CollisionManager::baseColliders_;

void CollisionManager::Init() { 


	//JSON関連****************************************************************
	globalParameter_ = GlobalParameter::GetInstance();
	const char* groupName = "CollisionManager";
	// グループを追加
	globalParameter_->CreateGroup(groupName);
	globalParameter_->AddItem(groupName, "isColliderVisible", isColliderVisible_);
}

void CollisionManager::AddCollider(BaseCollider* collider) {
	if (AABBCollider* aabbCollider = dynamic_cast<AABBCollider*>(collider)) {
		aabbCollider->Init();
		baseColliders_.push_back(collider);
	}
	else if (OBBCollider* obbCollider = dynamic_cast<OBBCollider*>(collider)) {
		obbCollider->Init();	
		baseColliders_.push_back(collider);
	}
	
}

void CollisionManager::RemoveCollider(BaseCollider* collider) {
	baseColliders_.remove(collider);  // リストから削除
}

void CollisionManager::Reset() {
	// リストを空っぽにする
	baseColliders_.clear();
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
	for (BaseCollider* baseCollider : baseColliders_) {
		baseCollider->UpdateWorldTransform();
	}	
}

void CollisionManager::Draw(const ViewProjection& viewProjection) {
	// 非表示なら抜ける
	if (!isColliderVisible_) {
		return;
	}
	//全てのコライダーを描画する
	for (BaseCollider* baseCollider : baseColliders_) {
		baseCollider->DrawDebugCube(viewProjection);
	}
}

// CheckCollisionPairを改造
void CollisionManager::CheckCollisionPair(BaseCollider* colliderA, BaseCollider* colliderB) {
	bool collisionDetected = false;

	// dynamic_castでAABBとOBBに変換して各ケースで判定
	if (auto* aabbA = dynamic_cast<AABBCollider*>(colliderA)) {
		if (auto* aabbB = dynamic_cast<AABBCollider*>(colliderB)) {
			// AABB同士の当たり判定
			collisionDetected = IsCollision(aabbA->GetAABB(), aabbB->GetAABB());
		}
		else if (auto* obbB = dynamic_cast<OBBCollider*>(colliderB)) {
			// AABBとOBBの当たり判定
			collisionDetected = IsCollision(obbB->GetOBB(), aabbA->GetAABB());
		}
	}
	else if (auto* obbA = dynamic_cast<OBBCollider*>(colliderA)) {
		if (auto* obbB = dynamic_cast<OBBCollider*>(colliderB)) {
			// OBB同士の当たり判定
			collisionDetected = IsCollision(obbA->GetOBB(), obbB->GetOBB());
		}
		else if (auto* aabbB = dynamic_cast<AABBCollider*>(colliderB)) {
			// OBBとAABBの当たり判定
			collisionDetected = IsCollision(obbA->GetOBB(), aabbB->GetAABB());
		}
	}

	if (collisionDetected) {
		HandleCollision(colliderA, colliderB);
	}
	else {
		// 衝突していない場合に終了処理を呼ぶ
		HandleCollisionExit(colliderA, colliderB);
	}
}


// コリジョン処理を分ける関数
void CollisionManager::HandleCollision(BaseCollider* colliderA, BaseCollider* colliderB) {
	// 衝突中の処理
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

// 衝突終了処理
void CollisionManager::HandleCollisionExit(BaseCollider* colliderA, BaseCollider* colliderB) {
	if (colliderA->GetIsColliding()) {
		colliderA->OnCollisionExit(colliderB);
		colliderA->SetColliding(false);
	}
	if (colliderB->GetIsColliding()) {
		colliderB->OnCollisionExit(colliderA);
		colliderB->SetColliding(false);
	}
}

void CollisionManager::CheckAllCollisions() {
	// リスト内のペアを総当たり
	std::list<BaseCollider*>::iterator itrA = baseColliders_.begin();
	for (; itrA != baseColliders_.end(); ++itrA) {
		// イテレータAからコライダーAを取得する
		BaseCollider* colliderA = *itrA;//ダブルポインタから中身のポインタを取り出す処理

		// イテレーターBはイテレータAの次の要素から回す（重複判定を回避）
		std::list<BaseCollider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != baseColliders_.end(); ++itrB) {
			BaseCollider* colliderB = *itrB;//ダブルポインタから中身のポインタを取り出す処理

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
