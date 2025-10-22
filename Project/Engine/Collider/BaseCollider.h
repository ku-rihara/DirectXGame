#pragma once

#include "3D/Line3D.h"
#include "3d/WorldTransform.h"
#include "Vector3.h"
#include <Vector4.h>

/// <summary>
///  Collider基底クラス
/// </summary>
class BaseCollider {
public:
    BaseCollider()          = default;
    virtual ~BaseCollider() = default;
    /// ===================================================
    /// public  method
    /// ===================================================

    virtual void Init() = 0; //< 初期化

    /// <summary>
    /// デバッグキューブの描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    virtual void DrawDebugCube(const ViewProjection& viewProjection) = 0;

    /// <summary>
    /// 衝突開始時の処理
    /// </summary>
    /// <param name="other">相手のコライダー</param>
    virtual void OnCollisionEnter([[maybe_unused]] BaseCollider* other);

    /// <summary>
    /// 衝突中の処理
    /// </summary>
    /// <param name="other">相手のコライダー</param>
    virtual void OnCollisionStay([[maybe_unused]] BaseCollider* other);

    /// <summary>
    /// 衝突終了時の処理
    /// </summary>
    /// <param name="other">相手のコライダー</param>
    virtual void OnCollisionExit([[maybe_unused]] BaseCollider* other);

    virtual void UpdateWorldTransform() = 0; //< ワールド変換の更新
    virtual Vector3 GetCollisionPos() const; //< 中心座標取得
    void ReverseNormalColor(); //< ラインカラーの反転

protected:
    /// ===================================================
    /// protected variant
    /// ===================================================

    WorldTransform cTransform_; //< ワールド変換
    uint32_t typeID_  = 0u; //< コリジョンのタイプID
    bool isColliding_ = false; //< 衝突フラグ
    Line3D debugLine_; //< デバッグ表示用Line
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