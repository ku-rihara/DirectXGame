#pragma once

#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
//class
#include "Colider/Collider.h"
#include "GrobalParamater/GlobalParameter.h"
/// std
#include<list>
#include<memory>

class CollisionManager {
private:
	// コライダーリスト
	static std::list<Collider*> colliders_;
	// コリジョンスフィア可視化
	GlobalParameter* globalParameter_;
	bool isColliderVisible_;

public:
	// 初期化、更新、描画
	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	// コリジョンスフィアのワールド行列の更新
	void UpdateWorldTransform();
	// リセット
	void Reset();
	// コライダーリスト登録
	static	void AddCollider(Collider* collider);

static	void RemoveCollider(Collider* collider);
	/// <summary>
	/// コライダー2つの衝突判定と応答
	/// </summary>
	/// <param name="コライダーA"></param>
	/// <param name="コライダーB"></param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
	// 全ての当たり判定をチェック
	void CheckAllCollisions();

	void ApplyGlobalParameter();
};