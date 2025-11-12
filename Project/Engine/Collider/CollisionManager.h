#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"
/// std
#include <list>
#include <memory>

class BaseCollider;
/// <summary>
///  Collision管理クラス
/// </summary>
class CollisionManager {
public:
    CollisionManager()  = default;
    ~CollisionManager() = default;

    void Init(); //< 初期化
    void Update(); //< 更新
    void LineAllSet();
   
    /// <summary>
    /// コライダーリストに登録
    /// </summary>
    /// <param name="collider">コライダー</param>
    static void AddCollider(BaseCollider* collider);

    /// <summary>
    /// コライダーリストから解除
    /// </summary>
    /// <param name="collider">コライダー</param>
    static void RemoveCollider(BaseCollider* collider);

    /// <summary>
    /// ペアの衝突チェック
    /// </summary>
    /// <param name="colliderA">コライダーA</param>
    /// <param name="colliderB">コライダーB</param>
    void CheckCollisionPair(BaseCollider* colliderA, BaseCollider* colliderB);

    /// <summary>
    /// 衝突処理
    /// </summary>
    /// <param name="colliderA">コライダーA</param>
    /// <param name="colliderB">コライダーB</param>
    void HandleCollision(BaseCollider* colliderA, BaseCollider* colliderB);

    /// <summary>
    /// 衝突終了処理
    /// </summary>
    /// <param name="colliderA">コライダーA</param>
    /// <param name="colliderB">コライダーB</param>
    void HandleCollisionExit(BaseCollider* colliderA, BaseCollider* colliderB);

    void UpdateWorldTransform(); //< ワールド変換の更新
    void Reset();                //< リセット
    void CheckAllCollisions();   //< 全ての衝突チェック
    void ApplyGlobalParameter(); //< グローバルパラメータの適用

public:
    struct PairHash {
        template <typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
        }
    };

private:
    // コライダーリスト
    static std::list<BaseCollider*> baseColliders_;
    // 衝突状態をペアごとに管理するマップ
    std::unordered_map<std::pair<BaseCollider*, BaseCollider*>, bool, PairHash> collisionStates_;

    // globalParameter
    GlobalParameter* globalParameter_;
    std::string groupName_ = "CollisionManager";
    bool isColliderVisible_;
};