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

// 定数バッファ用データ構造体
struct ConstBufferDataWorldTransform {
    Matrix4x4 matWorld;
};

enum class BillboardType {
    XYZ, // xyz
    X, // x
    Y, // y
    Z, // z
};

struct AdaptRotate {
    bool isX;
    bool isY;
    bool isZ;
};

enum class RotateOder {
    XYZ,
    Quaternion,
};

class Object3DAnimation;
class WorldTransform {

public:
    WorldTransform()  = default;
    ~WorldTransform() = default;

    void Init();

    void UpdateMatrix();

    Vector3 LookAt(const Vector3& direction) const;

    void BillboardUpdateMatrix(const ViewProjection& viewProjection, const BillboardType& billboardAxis = BillboardType::XYZ, const AdaptRotate& adaptRotate = {false, false, false});

private:
    void TransferMatrix();
    void UpdateAffineMatrix();
    void ClearParentJoint();
    void UpdateMatrixWithJoint();
    bool HasParentJoint() const;

public:
    // SRT,Q
    Vector3 scale_         = {1, 1, 1};
    Vector3 rotation_      = {};
    Vector3 translation_   = {};
    Quaternion quaternion_ = {};

    // matrix
    Matrix4x4 matWorld_;

    const WorldTransform* parent_ = nullptr;
    RotateOder rotateOder_        = RotateOder::XYZ;

private:
    // animation parent
    const Object3DAnimation* parentAnimation_ = nullptr;
    int32_t parentJointIndex_                 = -1;
    std::string parentJointName_;

    Matrix4x4 billboardMatrix_;
    Matrix4x4 backToFrontMatrix_;

    // コピー禁止
    WorldTransform(const WorldTransform&)            = delete;
    WorldTransform& operator=(const WorldTransform&) = delete;

public:
    void SetParent(const WorldTransform* parent);
    void SetParentJoint(const Object3DAnimation* animation, const std::string& jointName);

    Vector3 GetLocalPos() const;
    Vector3 GetWorldPos() const;

    // GetVector
    Vector3 GetRightVector() const;
    Vector3 GetUpVector() const;
    Vector3 GetForwardVector() const;

public:
    // ムーブコンストラクタ
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

    // ムーブ代入演算子
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
