#pragma once

#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
//class
#include "Collider/AABBCollider.h"
#include "Collider/OBBCollider.h"

#include"utility/Editor/GlobalParameter.h"
/// std
#include<list>
#include<memory>

class CollisionManager {
public:
	struct PairHash {
		template <typename T1, typename T2>
		std::size_t operator()(const std::pair<T1, T2>& p) const {
			return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
		}
	};
private:
	// コライダーリスト
	static std::list<BaseCollider*> baseColliders_;/// AABB
	// 衝突状態をペアごとに管理するマップ
	std::unordered_map<std::pair<BaseCollider*, BaseCollider*>, bool, PairHash> collisionStates_;

	// コリジョンスフィア可視化
	GlobalParameter* globalParameter_;
	bool isColliderVisible_;

public:
	/// ===================================================
   /// public  method
   /// ===================================================

	// 初期化、更新、描画
	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void UpdateWorldTransform();	// コリジョンのワールド行列の更新

	// リセット
	void Reset();

	// コライダーリスト登録
	static	void AddCollider(BaseCollider* collider);
	/// リスト除外
	static	void RemoveCollider(BaseCollider* collider);

	/// コライダー2つの衝突判定と応答
	void CheckCollisionPair(BaseCollider* colliderA, BaseCollider* colliderB);
	// 全ての当たり判定をチェック
	void CheckAllCollisions();

	void HandleCollision(BaseCollider* colliderA, BaseCollider* colliderB);
	void HandleCollisionExit(BaseCollider* colliderA, BaseCollider* colliderB);

	void ApplyGlobalParameter();
};