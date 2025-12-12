#include "ViewProjection.h"

#include "Dx/DirectXCommon.h"
#include "MathFunction.h"
#include "WorldTransform.h"
#include <cassert>

namespace KetaEngine {

void ViewProjection::Init() {

    // 定数バッファ生成
    CreateConstantBuffer();
    // マッピング
    Map();
    // 初期クォータニオンを設定
    quaternion_ = Quaternion::Identity();
    // ビュー行列の更新
    UpdateViewMatrix();
    // 射影行列の更新
    UpdateProjectionMatrix();
    // 行列の転送
    TransferMatrix();
}

void ViewProjection::CreateConstantBuffer() {
    // デバイスの取得
    Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXCommon::GetInstance()->GetDevice();
    // 定数バッファのサイズを計算
    const UINT bufferSize = sizeof(ConstBufferDataViewProjection);

    // 定数バッファを生成
    constBuffer_ = DirectXCommon::GetInstance()->CreateBufferResource(device, bufferSize);
}

void ViewProjection::Map() {
    // 定数バッファのマッピング
    D3D12_RANGE readRange = {};
    HRESULT hr            = constBuffer_->Map(0, &readRange, reinterpret_cast<void**>(&constMap));
    assert(SUCCEEDED(hr));

    if (FAILED(hr)) {
        OutputDebugStringA("Map failed.\n");
    }
}

void ViewProjection::TransferMatrix() {
    // 定数バッファに行列データを転送する
    constMap->view       = matView_;
    constMap->projection = matProjection_;
    constMap->cameraPos  = translation_;
}

void ViewProjection::UpdateMatrix() {

    //  ビュー行列の更新
    UpdateViewMatrix();
    // 射影行列の更新
    UpdateProjectionMatrix();
    // 行列の転送
    TransferMatrix();
}

void ViewProjection::UpdateViewMatrix() {
    //  ローカル回転
    Vector3 finalRotation  = GetFinalRotation();
    Matrix4x4 rotateMatrix = MakeRotateMatrix(finalRotation);

    // ローカル平行移動
    Vector3 transformedPosOffset = TransformNormal(positionOffset_, rotateMatrix);
    Vector3 finalLocalPos        = translation_ + transformedPosOffset;
    Matrix4x4 translateMatrix    = MakeTranslateMatrix(finalLocalPos);

    // ローカル変換
    Matrix4x4 localCameraMatrix = rotateMatrix * translateMatrix;

    if (parent_) {
        cameraMatrix_ = localCameraMatrix * parent_->matWorld_;
    } else {
        cameraMatrix_ = localCameraMatrix;
    }

    // ビュー行列は逆行列
    matView_ = Inverse(cameraMatrix_);
}

void ViewProjection::UpdateProjectionMatrix() {
    if (projectionType_ == ProjectionType::PERSPECTIVE) {
        // 透視投影
        matProjection_ = MakePerspectiveFovMatrix(ToRadian(fovAngleY_), aspectRatio_, nearZ_, farZ_);
    } else {
        // 平行投影
        matProjection_ = MakeOrthographicMatrix(-orthoWidth_ / 2.0f, orthoHeight_ / 2.0f, orthoWidth_ / 2.0f, -orthoHeight_ / 2.0f, 1.0f, 100.0f);
    }
}

Vector3 ViewProjection::GetWorldPos() const {
    return Vector3(
        cameraMatrix_.m[3][0], // X成分
        cameraMatrix_.m[3][1], // Y成分
        cameraMatrix_.m[3][2] // Z成分
    );
}

Vector3 ViewProjection::GetFinalPosition() const {
    Vector3 finalRotation             = GetFinalRotation();
    Matrix4x4 rotateMatrix            = MakeRotateMatrix(finalRotation);
    Vector3 transformedPositionOffset = TransformNormal(positionOffset_, rotateMatrix);

    return translation_ + transformedPositionOffset;
}

Vector3 ViewProjection::GetFinalRotation() const {
    return rotation_ + rotationOffset_;
}

Matrix4x4 ViewProjection::GetBillboardMatrix() const {
    // カメラ行列から回転成分のみを取得
    Matrix4x4 billboardMatrix = cameraMatrix_;

    // 平行移動成分をクリア
    billboardMatrix.m[3][0] = 0.0f;
    billboardMatrix.m[3][1] = 0.0f;
    billboardMatrix.m[3][2] = 0.0f;

    return billboardMatrix;
}

void ViewProjection::ClearParent() {
    parent_ = nullptr;
}

Vector3 ViewProjection::GetForward() const {

    return Vector3(
        cameraMatrix_.m[2][0], // X成分
        cameraMatrix_.m[2][1], // Y成分
        cameraMatrix_.m[2][2] // Z成分
    );
}

}