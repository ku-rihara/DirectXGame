#include "CollisionManager.h"

#include "Collider/AABBCollider.h"
#include "Collider/OBBCollider.h"
#include "Box.h"

// 静的メンバ変数の初期化
std::list<BaseCollider*> CollisionManager::baseColliders_;

void CollisionManager::Init() { 


	globalParameter_ = GlobalParameter::GetInstance();
	const char* groupName = "CollisionManager";

	// グループを追加
	globalParameter_->CreateGroup(groupName);

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
	baseColliders_.clear();
	collisionStates_.clear();  // 状態リセット
}


void CollisionManager::Update() { 
	
	//imguiからパラメータを取得
	ApplyGlobalParameter();

	UpdateWorldTransform();

	CheckAllCollisions();

	LineAllSet();
}

void CollisionManager::UpdateWorldTransform() {
	
	//非表示なら抜ける
	if (!isColliderVisible_) {
		return;
	}
	//全てのコライダーについて行列更新をする
	for (BaseCollider* baseCollider : baseColliders_) {
		baseCollider->UpdateWorldTransform();
		baseCollider->ReverseNormalColor();
	}	
}

void CollisionManager::LineAllSet() {
#ifdef _DEBUG

	// 非表示なら抜ける
	if (!isColliderVisible_) {
		return;
	}
	//全てのコライダーを描画する
	for (BaseCollider* baseCollider : baseColliders_) {
		baseCollider->SetDebugCube();
	}
#endif // _DEBUG

}

// CheckCollisionPairを改造
void CollisionManager::CheckCollisionPair(BaseCollider* colliderA, BaseCollider* colliderB) {

	if (!colliderA->GetIsCollision() || !colliderB->GetIsCollision()) {
		return;
	}

	bool collisionDetected = false;

	// dynamic_cast を使用して AABB と OBB の判定
	if (auto* aabbA = dynamic_cast<AABBCollider*>(colliderA)) {
		if (auto* aabbB = dynamic_cast<AABBCollider*>(colliderB)) {
			collisionDetected = IsCollision(aabbA->GetAABB(), aabbB->GetAABB());
		}
		else if (auto* obbB = dynamic_cast<OBBCollider*>(colliderB)) {
			collisionDetected = IsCollision(obbB->GetOBB(), aabbA->GetAABB());
		}
	}
	else if (auto* obbA = dynamic_cast<OBBCollider*>(colliderA)) {
		if (auto* obbB = dynamic_cast<OBBCollider*>(colliderB)) {
			collisionDetected = IsCollision(obbA->GetOBB(), obbB->GetOBB());
		}
		else if (auto* aabbB = dynamic_cast<AABBCollider*>(colliderB)) {
			collisionDetected = IsCollision(obbA->GetOBB(), aabbB->GetAABB());
		}
	}

	// ペアをキーにして状態を管理
	auto pair = std::make_pair(colliderA, colliderB);
	if (collisionDetected) {
		if (collisionStates_[pair] == false) {
			HandleCollision(colliderA, colliderB);  // 新しい衝突
		}
		else {
			// 既存の衝突継続
			colliderA->OnCollisionStay(colliderB);
			colliderB->OnCollisionStay(colliderA);
		}
		collisionStates_[pair] = true;  // 衝突中に設定
	}
	else {
		if (collisionStates_[pair] == true) {
			HandleCollisionExit(colliderA, colliderB);  // 衝突終了
		}
		collisionStates_[pair] = false;  // 衝突なしに設定
	}
}

// コリジョン処理を分ける関数
void CollisionManager::HandleCollision(BaseCollider* colliderA, BaseCollider* colliderB) {
	// **必ず OnCollisionEnter を呼ぶ**
	colliderA->OnCollisionEnter(colliderB);
	colliderB->OnCollisionEnter(colliderA);

	// その後、継続処理も実行
	colliderA->OnCollisionStay(colliderB);
	colliderB->OnCollisionStay(colliderA);

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
	
	const char* groupName = "CollisionManager";

	isColliderVisible_ = globalParameter_->GetValue<bool>(groupName, "isColliderVisible");
}
