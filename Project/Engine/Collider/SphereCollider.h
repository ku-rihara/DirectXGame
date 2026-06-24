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

    void Init() override;
    void SetDebugCube() override;
    void UpdateWorldTransform() override;
    ColliderShape GetShape() const override { return ColliderShape::Sphere; }

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
