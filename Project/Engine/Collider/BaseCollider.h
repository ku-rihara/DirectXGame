#pragma once

#include "3D/Line3D.h"
#include "3d/WorldTransform.h"
#include "Vector3.h"
#include <Vector4.h>

class BaseCollider {
public:
    BaseCollider()          = default;
    virtual ~BaseCollider() = default;
    /// ===================================================
    /// public  method
    /// ===================================================

    /// 初期化、更新、描画
    virtual void Init()                                              = 0;
    virtual void UpdateWorldTransform()                              = 0;
    virtual void DrawDebugCube(const ViewProjection& viewProjection) = 0;
    /// 中心座標取得
    virtual Vector3 GetCollisionPos() const;

    /// 各種当たり判定
    virtual void OnCollisionEnter([[maybe_unused]] BaseCollider* other);
    virtual void OnCollisionStay([[maybe_unused]] BaseCollider* other);
    virtual void OnCollisionExit([[maybe_unused]] BaseCollider* other);

    void ReverseNormalColor();

protected:
    /// ===================================================
    /// protected variant
    /// ===================================================

    WorldTransform cTransform_;             //< ワールド変換
    uint32_t typeID_  = 0u;                 //< コリジョンのタイプID
    bool isColliding_ = false;              //< 衝突フラグ
    Line3D debugLine_;                      //< デバッグ表示用Line
    Vector4 lineColor_ = Vector4::kWHITE(); //< デバッグ表示用のラインカラー

    bool isAdaptCollision = true;

public:
    /// ===================================================
    /// getter  methods
    /// ===================================================
    const uint32_t& GetTypeID() const { return typeID_; }
    const bool& GetIsColliding() const { return isColliding_; }
    const bool& GetIsCollision() const { return isAdaptCollision; }
    /// ===================================================
    /// setter  methods
    /// ===================================================
    void SetTypeID(const uint32_t& typeID) { typeID_ = typeID; }
    void SetColliding(const bool& colliding) { isColliding_ = colliding; }
    void SetIsAdaptCollision(const bool& is) { isAdaptCollision = is; }
};
