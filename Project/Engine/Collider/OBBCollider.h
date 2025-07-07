#pragma once

#include "BaseCollider.h"
#include "Vector3.h"
#include "Box.h"  

class OBBCollider : public BaseCollider {
private:
    /// ===================================================
    /// private variant
    /// ===================================================
    OBB obb_;  // OBB構造体

public:
    OBBCollider();
    ~OBBCollider();

    /// ===================================================
    /// public method
    /// ===================================================

    // 初期化処理
    void Init() override;
    void UpdateWorldTransform()override;
    void DrawDebugCube(const ViewProjection& viewProjection)override;
    void DrawOBBLine(const OBB& obb, const Vector4& color);
  
    virtual Vector3 GetCollisionPos() const override;
    virtual Vector3 GetRotate() const;

    /// ===================================================
    /// getter methods
    /// ===================================================
    OBB GetOBB() const { return obb_; }

    /// ===================================================
    /// setter methods
    /// ===================================================
    void SetCollisionSize(const Vector3& size) { obb_.size = size; }
};
