#pragma once
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include <d3d12.h>
#include <list>
#include <wrl.h>

// 定数バッファ用データ構造体
struct ConstBufferDataWorldTransform {
    Matrix4x4 matWorld; // ローカル → ワールド変換行列
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

/// <summary>
/// ワールド変換データ
/// </summary>
class WorldTransform {

public:
    WorldTransform();
    ~WorldTransform();

    void Init();

    void CreateConstantBuffer();
    void Map();

    void UpdateMatrix();
  

    Vector3 LookAt(const Vector3& direction) const;

    void BillboardUpdateMatrix(const ViewProjection& viewProjection, const BillboardType& billboardAxis = BillboardType::XYZ, const AdaptRotate& adaptRotate = {false, false, false});

private:
    void TransferMatrix();
    void UpdateAffineMatrix();
   
public:
    // ローカルスケール
    Vector3 scale_ = {1, 1, 1};
    // ローカル回転角
    Vector3 rotation_ = {};
    // ローカル座標
    Vector3 translation_   = {};
    Quaternion quaternion_ = {};
    // ローカル→ワールド変換行列
    Matrix4x4 matWorld_;
    // 親となるワールド変換へのポインタ
    const WorldTransform* parent_ = nullptr;
    RotateOder rotateOder_        = RotateOder::XYZ;

private:
    Matrix4x4 billboardMatrix_;
    Matrix4x4 backToFrontMatrix_;

    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
    // マッピング済みアドレス
    ConstBufferDataWorldTransform* constMap = nullptr;
    // コピー禁止
    WorldTransform(const WorldTransform&)            = delete;
    WorldTransform& operator=(const WorldTransform&) = delete;

public:
    void SetParent(const WorldTransform* parent);

    Vector3 GetLocalPos() const;
    Vector3 GetWorldPos() const;

   
    /// 定数バッファの取得
    const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }

    Vector3 GetRightVector() const {
        return Vector3(matWorld_.m[0][0], matWorld_.m[1][0], matWorld_.m[2][0]);
    }

    Vector3 GetUpVector() const {
        return Vector3(matWorld_.m[0][1], matWorld_.m[1][1], matWorld_.m[2][1]);
    }

    Vector3 GetForwardVector() const {
        return Vector3(matWorld_.m[0][2], matWorld_.m[1][2], matWorld_.m[2][2]);
    }


    public:
    // ムーブコンストラクタを追加
    WorldTransform(WorldTransform&& other) noexcept
        : scale_(std::move(other.scale_)),
          rotation_(std::move(other.rotation_)),
          translation_(std::move(other.translation_)),
          matWorld_(std::move(other.matWorld_)),
          parent_(other.parent_),
          billboardMatrix_(std::move(other.billboardMatrix_)),
          backToFrontMatrix_(std::move(other.backToFrontMatrix_)),
          constBuffer_(std::move(other.constBuffer_)),
          constMap(other.constMap) {
        // ムーブ後に other を初期化（必要に応じて）
        other.constMap = nullptr; // など
    }

    // ムーブ代入演算子を追加
    WorldTransform& operator=(WorldTransform&& other) noexcept {
        if (this != &other) {
            scale_             = std::move(other.scale_);
            rotation_          = std::move(other.rotation_);
            translation_       = std::move(other.translation_);
            matWorld_          = std::move(other.matWorld_);
            parent_            = other.parent_;
            billboardMatrix_   = std::move(other.billboardMatrix_);
            backToFrontMatrix_ = std::move(other.backToFrontMatrix_);
            constBuffer_       = std::move(other.constBuffer_);
            constMap           = other.constMap;

            // ムーブ後に other を初期化
            other.constMap = nullptr; // など
        }
        return *this;
    }
};
