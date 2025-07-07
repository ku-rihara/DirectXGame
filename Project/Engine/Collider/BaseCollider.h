#pragma once

#include "Vector3.h"
#include "3d/WorldTransform.h"
#include"3D/Line3D.h"
#include <Vector4.h>

class BaseCollider {
protected:
    /// ===================================================
    /// protected variant
    /// ===================================================
    
    WorldTransform cTransform_;            // ワールド変換
    uint32_t typeID_ = 0u;                 // コリジョンのタイプID
    bool isColliding_ = false;             // 衝突フラグ
    Line3D debugLine_;                     // デバッグ表示用Line
    Vector4 lineColor_ = Vector4::kWHITE(); // デバッグ表示用のラインカラー

   bool isCollision_=true;
public:
    BaseCollider() = default;
    virtual ~BaseCollider() = default;
    /// ===================================================
    /// public  method
    /// ===================================================
    
    /// 初期化、更新、描画
    virtual void Init() = 0;
    virtual void UpdateWorldTransform() = 0;
    virtual void DrawDebugCube(const ViewProjection& viewProjection)=0;
    /// 中心座標取得
    virtual Vector3 GetCollisionPos() const;

    /// 各種当たり判定
    virtual void OnCollisionEnter([[maybe_unused]] BaseCollider* other);
    virtual void OnCollisionStay([[maybe_unused]] BaseCollider* other);
    virtual void OnCollisionExit([[maybe_unused]] BaseCollider* other);

    void ReverseNormalColor();
    /// ===================================================
   /// getter  methods
   /// ===================================================
    uint32_t GetTypeID() const { return typeID_; }
    bool GetIsColliding() const { return isColliding_; }
    bool GetIsCollision()const { return isCollision_; }
    /// ===================================================
    /// setter  methods
    /// ===================================================
    void SetTypeID(uint32_t typeID) { typeID_ = typeID; }
    void SetColliding(bool colliding) { isColliding_ = colliding; }
    void SetIsCollision(bool is) { isCollision_ = is; }
};
