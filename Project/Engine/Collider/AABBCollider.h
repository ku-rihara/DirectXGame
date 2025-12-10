#pragma once

#include "BaseCollider.h"
#include "Box.h"

/// <summary>
///  AABBコライダー
/// </summary>
namespace KetaEngine {

class AABBCollider : public BaseCollider {
public:
    AABBCollider();
    ~AABBCollider();
    /// ===================================================
    /// public  method
    /// ===================================================

    void Init() override; //< 初期化
    void SetDebugCube() override; //<デバッグキューブの描画セット
    
    void UpdateWorldTransform() override;             //< ワールド変換の更新
    virtual Vector3 GetCollisionPos() const override; //< 衝突座標の取得

private:
    /// ===================================================
    /// private variant
    /// ===================================================
    Vector3 collisionScale_ = {1.5f, 1.5f, 1.5f}; // AABBのサイズ
    AABB aabb_; // AABB範囲

public:
    ///=========================================================================================
    ///  getter method
    ///=========================================================================================

    const AABB& GetAABB() const { return aabb_; }
    const Vector3& GetCollisonScale() const { return collisionScale_; }

    ///=========================================================================================
    ///  setter method
    ///=========================================================================================
    void SetCollisionScale(const Vector3& radius) { collisionScale_ = radius; }
};

}; // KetaEngine
