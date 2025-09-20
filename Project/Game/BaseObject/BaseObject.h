#pragma once
#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

// std
#include <string>

class BaseObject {
protected:
    /// ===================================================
    ///  protected variaus
    /// ===================================================

    // モデル配列データ
    std::unique_ptr<Object3d> obj3d_;
    // ベースのワールド変換データ
    WorldTransform baseTransform_;

public:
    virtual ~BaseObject() = default;
    /// ===================================================
    /// public method
    /// ===================================================

    // 初期化、更新、描画
    virtual void Init();
    virtual void Update();

    // 中心座標取得
    virtual Vector3 GetWorldPosition() const;

    /// ===================================================
    /// getter
    /// ===================================================
    const WorldTransform& GetTransform()const { return baseTransform_; }
    Vector3 GetScale() const { return baseTransform_.scale_; }
    /// ===================================================
    /// setter
    /// ===================================================
    void SetParent(WorldTransform* parent);
    void SetColor(const Vector4& color);
    void SetWorldPosition(const Vector3& pos) { baseTransform_.translation_ = pos; }
    void SetWorldPositionY(const float& pos) { baseTransform_.translation_.y = pos; }
    void SetWorldPositionX(const float& pos) { baseTransform_.translation_.x = pos; }
    void SetWorldPositionZ(const float& pos) { baseTransform_.translation_.z = pos; }
    void AddPosition(const Vector3& pos) { baseTransform_.translation_ += pos; }
    void SetRotation(const Vector3& rotate) { baseTransform_.rotation_ = rotate; }
    void SetRotationY(const float& rotate) { baseTransform_.rotation_.y = rotate; }
    void SetRotationX(const float& rotate) { baseTransform_.rotation_.x = rotate; }
    void SetRotationZ(const float& rotate) { baseTransform_.rotation_.z = rotate; }
    void SetScale(const Vector3& scale) { baseTransform_.scale_ = scale; }
    void SetScaleX(const float& scale) { baseTransform_.scale_.x = scale; }
    void SetScaleY(const float& scale) { baseTransform_.scale_.y = scale; }
    void SetScaleZ(const float& scale) { baseTransform_.scale_.z = scale; }
};
