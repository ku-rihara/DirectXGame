#pragma once

#include "BaseCollider.h"
#include "Box.h"
#include "Vector3.h"

/// <summary>
/// OBBコライダークラス
/// </summary>
class OBBCollider : public BaseCollider {
public:
    OBBCollider();
    ~OBBCollider();

    /// ===================================================
    /// public method
    /// ===================================================

    void Init() override; //< 初期化処理

    /// <summary>
    /// デバッグキューブの描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DrawDebugCube(const ViewProjection& viewProjection) override;

    /// <summary>
    /// OBBのラインを描画
    /// </summary>
    /// <param name="obb">OBB</param>
    /// <param name="color">色</param>
    void DrawOBBLine(const OBB& obb, const Vector4& color);

    void UpdateWorldTransform() override; //< ワールド変換の更新
    virtual Vector3 GetCollisionPos() const override; //< 衝突座標の取得
    virtual Vector3 GetRotate() const; //< 回転の取得

private:
    OBB obb_; // OBB構造体
public:
    // getter
    const OBB& GetOBB() const { return obb_; }
    // setter
    void SetCollisionSize(const Vector3& size) { obb_.size = size; }
};