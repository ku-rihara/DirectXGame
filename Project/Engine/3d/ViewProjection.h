#pragma once
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <d3d12.h>
#include <wrl.h>

// 定数バッファ用データ構造体
struct ConstBufferDataViewProjection {
    Matrix4x4 view;
    Matrix4x4 projection;
    Vector3 cameraPos;
};

class WorldTransform;
class ViewProjection {
public:
    enum class ProjectionType {
        PERSPECTIVE, // 透視投影
        ORTHOGRAPHIC // 平行投影
    };

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
    ConstBufferDataViewProjection* constMap          = nullptr;
    ViewProjection(const ViewProjection&)            = delete;
    ViewProjection& operator=(const ViewProjection&) = delete;

public:
    ViewProjection()  = default;
    ~ViewProjection() = default;

    /// 初期化
    void Init();
    /// 定数バッファ生成,マッピング
    void CreateConstantBuffer();
    void Map();

    /// 行列転送,更新
    void TransferMatrix();
    void UpdateMatrix();

    // ビュー、プロジェクション行列更新
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    // ペアレント解除
    void ClearParent();

private:
    const WorldTransform* parent_ = nullptr;

public:
    // S,R,T
    Vector3 scale_{1, 1, 1};
    Vector3 rotation_      = {0, 0, 0};
    Vector3 translation_   = {0, 0, 0};
    Quaternion quaternion_ = Quaternion::Identity();

    // オフセット値
    Vector3 positionOffset_ = {0, 0, 0};
    Vector3 rotationOffset_ = {0, 0, 0};

    float fovAngleY_   = 45.0f;
    float aspectRatio_ = (float)16 / 9;
    float nearZ_       = 0.1f;
    float farZ_        = 1000.0f;

    ProjectionType projectionType_ = ProjectionType::PERSPECTIVE;

    // 平行投影用パラメータ
    float orthoWidth_  = 150.0f;
    float orthoHeight_ = 150.0f;

    // matrix
    Matrix4x4 matView_;
    Matrix4x4 matProjection_;
    Matrix4x4 cameraMatrix_;

public:
    // getter
    Vector3 GetWorldPos() const;
    const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }
    const Matrix4x4& GetCameraMatrix() const { return cameraMatrix_; }
    Vector3 GetFinalPosition() const;
    Vector3 GetFinalRotation() const;
    Matrix4x4 GetBillboardMatrix() const;

    // setter
    void SetParent(const WorldTransform* parent) { parent_ = parent; }
};