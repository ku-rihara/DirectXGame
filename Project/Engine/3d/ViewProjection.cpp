#include "ViewProjection.h"

#include "Dx/DirectXCommon.h"
#include "MathFunction.h"
#include "WorldTransform.h"
#include <cassert>
#include <cmath>

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

void ViewProjection::ApplyLookAtMode(const Vector3& lookAtTarget, const Vector3& rotateOffset, const Vector3& posOffset) {
    // Y軸回転行列を作成
    Matrix4x4 rotateY = MakeRotateYMatrix(rotateOffset.y);

    // X軸回転
    Matrix4x4 rotateX = MakeRotateXMatrix(-rotateOffset.x);

    // 回転を合成
    Matrix4x4 rotateMatrix = rotateY * rotateX;

    // 回転を適用してカメラオフセット位置を計算
    Vector3 cameraOffset = TransformNormal(posOffset, rotateMatrix);

    // 注視点からのオフセットでカメラ位置を決定
    Vector3 cameraPos = lookAtTarget + cameraOffset;

    // ViewProjectionに適用
    translation_ = cameraPos;

    // カメラから注視点への方向ベクトルを計算
    Vector3 toTarget = lookAtTarget - cameraPos;

    if (toTarget.Length() > 0.001f) {
        toTarget = toTarget.Normalize();

        // Y軸回転を計算
        float actualYaw = std::atan2(toTarget.x, toTarget.z);

        // X軸回転を計算
        float horizontalLength = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
        float actualPitch      = std::atan2(-toTarget.y, horizontalLength);

        // 計算した回転を適用
        rotation_.y = actualYaw;
        rotation_.x = actualPitch;
        rotation_.z = 0.0f;
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