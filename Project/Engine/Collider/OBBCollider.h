#pragma once

#include "BaseCollider.h"
#include "Box.h"
#include "Vector3.h"

class OBBCollider : public BaseCollider {
public:
    OBBCollider();
    ~OBBCollider();

    /// ===================================================
    /// public method
    /// ===================================================

    // 初期化処理
    void Init() override;
    void UpdateWorldTransform() override;
    void DrawDebugCube(const ViewProjection& viewProjection) override;
    void DrawOBBLine(const OBB& obb, const Vector4& color);

    virtual Vector3 GetCollisionPos() const override;
    virtual Vector3 GetRotate() const;

private:
    OBB obb_; // OBB構造体
public:
    // getter
    const OBB& GetOBB() const { return obb_; }
    // setter
    void SetCollisionSize(const Vector3& size) { obb_.size = size; }
};
