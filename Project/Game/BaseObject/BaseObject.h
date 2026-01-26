#pragma once
#include "3d/Object3d/Object3d.h"
#include "3d/WorldTransform.h"

// std
#include <string>

/// <summary>
/// オブジェクトの基底クラス
/// </summary>
class BaseObject {
public:
    virtual ~BaseObject() = default;
    /// ===================================================
    /// public method
    /// ===================================================

    // 初期化、更新
    virtual void Init();
    virtual void Update();

    virtual Vector3 GetWorldPosition() const;

protected:
    /// ===================================================
    ///  protected variables
    /// ===================================================

    // モデル配列データ
    std::unique_ptr<KetaEngine::Object3d> obj3d_;
    // ベースのワールド変換データ
    KetaEngine::WorldTransform baseTransform_;

public:
    /// ===================================================
    /// getter
    /// ===================================================
    const KetaEngine::WorldTransform& GetBaseTransform() const { return baseTransform_; }
    const Vector3& GetScale() const { return baseTransform_.scale_; }
    KetaEngine::Object3d* GetObject3D() const { return obj3d_.get(); }
    /// ===================================================
    /// setter
    /// ===================================================
    void SetParent(KetaEngine::WorldTransform* parent);
    void SetColor(const Vector4& color);
    void SetWorldPosition(const Vector3& pos) { baseTransform_.translation_ = pos; }
    void SetWorldPositionY(float pos) { baseTransform_.translation_.y = pos; }
    void SetWorldPositionX(float pos) { baseTransform_.translation_.x = pos; }
    void SetWorldPositionZ(float pos) { baseTransform_.translation_.z = pos; }
    void AddPosition(const Vector3& pos) { baseTransform_.translation_ += pos; }
    void SetRotation(const Vector3& rotate) { baseTransform_.rotation_ = rotate; }
    void SetRotationY(float rotate) { baseTransform_.rotation_.y = rotate; }
    void SetRotationX(float rotate) { baseTransform_.rotation_.x = rotate; }
    void SetRotationZ(float rotate) { baseTransform_.rotation_.z = rotate; }
    void SetScale(const Vector3& scale) { baseTransform_.scale_ = scale; }
    void SetScaleX(float scale) { baseTransform_.scale_.x = scale; }
    void SetScaleY(float scale) { baseTransform_.scale_.y = scale; }
    void SetScaleZ(float scale) { baseTransform_.scale_.z = scale; }
};