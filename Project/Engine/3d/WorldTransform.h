#pragma once
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include <cstdint>
#include <d3d12.h>
#include <list>
#include <string>
#include <wrl.h>
#include <cstdint>
#include <string>

struct ConstBufferDataWorldTransform {
    Matrix4x4 matWorld;
};

enum class BillboardType {
    XYZ, //< XYZ軸ビルボード
    X, //< X軸ビルボード
    Y, //< Y軸ビルボード
    Z, //< Z軸ビルボード
};

struct AdaptRotate {
    bool isX;
    bool isY;
    bool isZ;
};

enum class RotateOder {
    XYZ, //< オイラー角
    Quaternion, //< クォータニオン
};

class Object3DAnimation;

/// <summary>
/// ワールド変換クラス
/// </summary>
class WorldTransform {

public:
    WorldTransform()  = default;
    ~WorldTransform() = default;

    void Init(); //< 初期化

    void UpdateMatrix(); //< 行列更新

    /// <summary>
    /// 指定方向を向く
    /// </summary>
    /// <param name="direction">方向ベクトル</param>
    /// <returns>ワールド方向ベクトル</returns>
    Vector3 LookAt(const Vector3& direction) const;

    /// <summary>
    /// ビルボード行列更新
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    /// <param name="billboardAxis">ビルボード軸タイプ</param>
    /// <param name="adaptRotate">回転適用設定</param>
    void BillboardUpdateMatrix(const ViewProjection& viewProjection, const BillboardType& billboardAxis = BillboardType::XYZ, const AdaptRotate& adaptRotate = {false, false, false});

private:
    void TransferMatrix(); //< 行列転送
    void UpdateAffineMatrix(); //< アフィン行列更新
    void ClearParentJoint(); //< ペアレントジョイントクリア
    void UpdateMatrixWithJoint(); //< ジョイントで行列更新
    bool HasParentJoint() const; //< ペアレントジョイントを持つか

public:
    Vector3 scale_ = Vector3::UnitVector();
    Vector3 rotation_;
    Vector3 translation_;
    Quaternion quaternion_;

    Matrix4x4 matWorld_; //< ワールド行列

    const WorldTransform* parent_ = nullptr;
    RotateOder rotateOder_        = RotateOder::XYZ;

private:
    const Object3DAnimation* parentAnimation_ = nullptr;
    int32_t parentJointIndex_                 = -1;
    std::string parentJointName_;

    Matrix4x4 billboardMatrix_;
    Matrix4x4 backToFrontMatrix_;

    WorldTransform(const WorldTransform&)            = delete;
    WorldTransform& operator=(const WorldTransform&) = delete;

public:
    /// <summary>
    /// ペアレント設定
    /// </summary>
    /// <param name="parent">親のWorldTransform</param>
    void SetParent(const WorldTransform* parent);

    /// <summary>
    /// ペアレントジョイント設定
    /// </summary>
    /// <param name="animation">アニメーション</param>
    /// <param name="jointName">ジョイント名</param>
    void SetParentJoint(const Object3DAnimation* animation, const std::string& jointName);

    Vector3 GetLocalPos() const; //< ローカル座標取得
    Vector3 GetWorldPos() const; //< ワールド座標取得

    Vector3 GetRightVector() const; //< 右方向ベクトル取得
    Vector3 GetUpVector() const; //< 上方向ベクトル取得
    Vector3 GetForwardVector() const; //< 前方向ベクトル取得


public:
    WorldTransform(WorldTransform&& other) noexcept
        : scale_(std::move(other.scale_)),
          rotation_(std::move(other.rotation_)),
          translation_(std::move(other.translation_)),
          quaternion_(std::move(other.quaternion_)),
          matWorld_(std::move(other.matWorld_)),
          parent_(other.parent_),
          rotateOder_(other.rotateOder_),
          parentAnimation_(other.parentAnimation_),
          parentJointIndex_(other.parentJointIndex_),
          parentJointName_(std::move(other.parentJointName_)),
          billboardMatrix_(std::move(other.billboardMatrix_)),
          backToFrontMatrix_(std::move(other.backToFrontMatrix_)) {

        other.parentAnimation_  = nullptr;
        other.parentJointIndex_ = -1;
        other.parentJointName_.clear();
    }

    WorldTransform& operator=(WorldTransform&& other) noexcept {
        if (this != &other) {
            scale_             = std::move(other.scale_);
            rotation_          = std::move(other.rotation_);
            translation_       = std::move(other.translation_);
            quaternion_        = std::move(other.quaternion_);
            matWorld_          = std::move(other.matWorld_);
            parent_            = other.parent_;
            rotateOder_        = other.rotateOder_;
            parentAnimation_   = other.parentAnimation_;
            parentJointIndex_  = other.parentJointIndex_;
            parentJointName_   = std::move(other.parentJointName_);
            billboardMatrix_   = std::move(other.billboardMatrix_);
            backToFrontMatrix_ = std::move(other.backToFrontMatrix_);

            other.parentAnimation_  = nullptr;
            other.parentJointIndex_ = -1;
            other.parentJointName_.clear();
        }
        return *this;
    }
};