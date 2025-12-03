#pragma once
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <d3d12.h>
#include <wrl.h>

struct ConstBufferDataViewProjection {
    Matrix4x4 view;
    Matrix4x4 projection;
    Vector3 cameraPos;
};

class WorldTransform;

/// <summary>
/// ビュープロジェクションクラス
/// </summary>
class ViewProjection {
public:
    enum class ProjectionType {
        PERSPECTIVE, //< 透視投影
        ORTHOGRAPHIC //< 平行投影
    };

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
    ConstBufferDataViewProjection* constMap          = nullptr;
    ViewProjection(const ViewProjection&)            = delete;
    ViewProjection& operator=(const ViewProjection&) = delete;

public:
    ViewProjection()  = default;
    ~ViewProjection() = default;

    void Init();                 //< 初期化
    void CreateConstantBuffer(); //< 定数バッファ生成
    void Map();                  //< マッピング

    // 行列転送、更新
    void TransferMatrix(); 
    void UpdateMatrix();   
    void UpdateViewMatrix();       
    void UpdateProjectionMatrix(); 

    // ペアレント解除
    void ClearParent();           

private:
    const WorldTransform* parent_ = nullptr;

public:
    Vector3 scale_{1, 1, 1};
    Vector3 rotation_      = {0, 0, 0};
    Vector3 translation_   = {0, 0, 0};
    Quaternion quaternion_ = Quaternion::Identity();

    Vector3 positionOffset_ = {0, 0, 0}; //< 位置オフセット
    Vector3 rotationOffset_ = {0, 0, 0}; //< 回転オフセット
  
    float fovAngleY_   = 45.0f;
    float aspectRatio_ = (float)16 / 9;
    float nearZ_       = 0.1f;
    float farZ_        = 1000.0f;

    ProjectionType projectionType_ = ProjectionType::PERSPECTIVE;

    float orthoWidth_  = 150.0f; //< 平行投影の幅
    float orthoHeight_ = 150.0f; //< 平行投影の高さ

    Matrix4x4 matView_; //< ビュー行列
    Matrix4x4 matProjection_; //< プロジェクション行列
    Matrix4x4 cameraMatrix_; //< カメラ行列

public:
    Vector3 GetWorldPos() const;
    Vector3 GetForward() const;
    const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }
    const Matrix4x4& GetCameraMatrix() const { return cameraMatrix_; }
    Vector3 GetFinalPosition() const;
    Vector3 GetFinalRotation() const;
    Matrix4x4 GetBillboardMatrix() const;

    void SetParent(const WorldTransform* parent) { parent_ = parent; }
};