#pragma once
#include<list>
#include<memory>
#include"Colider.h"
#include "Model.h"
#include "ViewProjection.h"
#include "GlobalParameter.h"

class CollisionManager {
private:
	GlobalParameter* globalParameter_;

	bool isColliderVisible_;
	// コライダー
	std::list<Colider*> colliders_;
	// デバッグ表示用モデル
	std::unique_ptr<Model> model_;

public:
	// 初期化
	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	// ワールドトランスフォーム
	void UpdateWorldTransform();
	// リセット
	void Reset();
	// コライダーリスト登録
	void AddCollider(Colider* collider);
	/// <summary>
	/// コライダー2つの衝突判定と応答
	/// </summary>
	/// <param name="コライダーA"></param>
	/// <param name="コライダーB"></param>
	void CheckCollisionPair(Colider* colliderA, Colider* colliderB);
	// 全ての当たり判定をチェック
	void CheckAllCollisions();

	void ApplyGlobalParameter();
};