#pragma once

#include "3d/WorldTransform.h"
#include "Collider/AABBCollider.h"
#include <string>

/// <summary>
/// AABBコリジョンボックスの基底クラス
/// </summary>
class BaseAABBCollisionBox : public AABBCollider {
public:
    BaseAABBCollisionBox()          = default;
    virtual ~BaseAABBCollisionBox() = default;

    /// ====================================================
    ///  public method
    /// ====================================================

    // 初期化、更新、描画
    virtual void Init();
    virtual void Update(const float& timeSpeed);
  
    /// <summary>
    /// 親トランスフォームの設定
    /// </summary>
    /// <param name="transform">親のワールドトランスフォーム</param>
    virtual void SetParentTransform(WorldTransform* transform);

    /// <summary>
    /// 衝突開始時コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    virtual void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;

    /// <summary>
    /// 衝突中コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    virtual void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

protected:
    // transform,offset
    WorldTransform transform_;
    Vector3 offset_;
    Vector3 size_;
    bool isCollision_ = true;

    std::string tag_;

public:
    /// getter
    virtual Vector3 GetCollisionPos() const override;
    const std::string& GetTag() const { return tag_; }

    /// setter
    void SetIsCollision(const bool& is) { isCollision_ = is; }
    void SetSize(const Vector3& size) { size_ = size; }
    void SetPosition(const Vector3& position) { transform_.translation_ = position; }
    void SetOffset(const Vector3& offset) { offset_ = offset; }
};