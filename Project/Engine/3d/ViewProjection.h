#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include <d3d12.h>
#include <wrl.h>

// 定数バッファ用データ構造体
struct ConstBufferDataViewProjection {
    Matrix4x4 view; // ビュー変換行列
    Matrix4x4 projection; // プロジェクション変換行列
    Vector3 cameraPos; // カメラ座標
};

class ViewProjection {
public:
    ViewProjection()  = default;
    ~ViewProjection() = default;

    /// 初期化
    void Init();
    /// 定数バッファ生成
    void CreateConstantBuffer();
    /// マッピング
    void Map();
    /// 行列転送
    void TransferMatrix();
    /// 行列の更新
    void UpdateMatrix();
    /// ビュー行列の更新
    void UpdateViewMatrix();
    /// 射影行列を更新する
    void UpdateProjectionMatrix();

public:
    // S,R,T
    Vector3 scale_{1, 1, 1};
    Vector3 rotation_    = {0, 0, 0};
    Vector3 translation_ = {0, 0, 0};

    float fovAngleY_   = 45.0f * 3.141592654f / 180.0f;
    float aspectRatio_ = (float)16 / 9;
    float nearZ_       = 0.1f;
    float farZ_        = 1000.0f;

    // matrix
    Matrix4x4 matView_;
    Matrix4x4 matProjection_;
    Matrix4x4 cameraMatrix_;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
    ConstBufferDataViewProjection* constMap          = nullptr;
    ViewProjection(const ViewProjection&)            = delete;
    ViewProjection& operator=(const ViewProjection&) = delete;

public: // getter
    Vector3 GetWorldPos() const;
    const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }
    const Matrix4x4& GetCameraMatrix() const { return cameraMatrix_; }
};
