#pragma once

#include "BaseCollider.h"
#include "Box.h"

/// <summary>
/// 球コライダークラス
/// </summary>
namespace KetaEngine {

class SphereCollider : public BaseCollider {
public:
    SphereCollider();
    ~SphereCollider();

    /// ===================================================
    /// public method
    /// ===================================================

    void Init() override;             //< 初期化処理
    void SetDebugCube() override;     //< デバッグ球のセット
    void UpdateWorldTransform() override; //< ワールド変換の更新
    virtual Vector3 GetCollisionPos() const override; //< 衝突座標の取得

private:
    Sphere sphere_; // 球構造体

public:
    // getter
    const Sphere& GetSphere() const { return sphere_; }
    float GetCollisionRadius() const { return sphere_.radius; }
    // setter
    void SetCollisionRadius(float radius) { sphere_.radius = radius; }
};

}; // KetaEngine
