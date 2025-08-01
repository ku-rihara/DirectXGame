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
    const WorldTransform& GetTransform() { return baseTransform_; }
    Vector3 GetScale() const { return baseTransform_.scale_; }
    /// ===================================================
    /// setter
    /// ===================================================
    void SetParent(WorldTransform* parent);
    void SetColor(const Vector4& color);
    void SetWorldPosition(Vector3 pos) { baseTransform_.translation_ = pos; }
    void SetWorldPositionY(float pos) { baseTransform_.translation_.y = pos; }
    void SetWorldPositionX(float pos) { baseTransform_.translation_.x = pos; }
    void SetWorldPositionZ(float pos) { baseTransform_.translation_.z = pos; }
    void AddPosition(Vector3 pos) { baseTransform_.translation_ += pos; }
    void SetRotation(Vector3 rotate) { baseTransform_.rotation_ = rotate; }
    void SetRotationY(float rotate) { baseTransform_.rotation_.y = rotate; }
    void SetRotationX(float rotate) { baseTransform_.rotation_.x = rotate; }
    void SetRotationZ(float rotate) { baseTransform_.rotation_.z = rotate; }
    void SetScale(Vector3 scale) { baseTransform_.scale_ = scale; }
    void SetScaleX(float scale) { baseTransform_.scale_.x = scale; }
    void SetScaleY(float scale) { baseTransform_.scale_.y = scale; }
    void SetScaleZ(float scale) { baseTransform_.scale_.z = scale; }
};
