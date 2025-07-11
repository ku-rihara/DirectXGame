#include "WorldTransform.h"
#include "Animation/ModelAnimation.h"
#include "Dx/DirectXCommon.h"
#include <assert.h>
#include <numbers>

void WorldTransform::Init() {

    scale_       = {1, 1, 1};
    rotation_    = {};
    translation_ = {};

    //// 定数バッファの生成
    // CreateConstantBuffer();
    //// マッピング
    // Map();
    //  行列の更新
    UpdateMatrix();
}

void WorldTransform::CreateConstantBuffer() {
    // デバイスの取得
    Microsoft::WRL::ComPtr<ID3D12Device> device = DirectXCommon::GetInstance()->GetDevice();
    // 定数バッファのサイズを計算
    const UINT bufferSize = sizeof(ConstBufferDataWorldTransform);

    // 定数バッファを生成
    constBuffer_ = DirectXCommon::GetInstance()->CreateBufferResource(device, bufferSize);
}

void WorldTransform::Map() {
    // 定数バッファのマッピング
    D3D12_RANGE readRange = {};
    HRESULT hr            = constBuffer_->Map(0, &readRange, reinterpret_cast<void**>(&constMap));
    if (FAILED(hr)) {
        // エラー処理
        OutputDebugStringA("ConstBuffer Map failed.\n");
    }
}

void WorldTransform::TransferMatrix() {

    if (constMap) {
        constMap->matWorld = matWorld_;
    }
}


void WorldTransform::UpdateMatrix() {
    // SRT更新
    UpdateAffineMatrix();

    // JointParent
    if (HasParentJoint()) {
        UpdateMatrixWithJoint();
    }
    // 通常のparent
    else if (parent_) {
        matWorld_ *= parent_->matWorld_;
    }

    // 定数バッファに転送する
    TransferMatrix();
}
void WorldTransform::BillboardUpdateMatrix(const ViewProjection& viewProjection, const BillboardType& billboardAxis, const AdaptRotate& adaptRotate) {
    // スケール、回転、平行移動行列を計算
    Matrix4x4 scaleMatrix     = MakeScaleMatrix(scale_);
    Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);

    // カメラ行列を取得
    Matrix4x4 cameraMatrix = viewProjection.GetCameraMatrix();

    // カメラ位置を取得し、オブジェクトとのベクトルを計算
    Vector3 cameraPos = viewProjection.GetWorldPos();
    Vector3 toCamera  = {cameraPos.x - translation_.x, 0.0f, cameraPos.z - translation_.z};
    toCamera          = (toCamera).Normalize();

    // ビルボード行列の計算
    switch (billboardAxis) {
    case BillboardType::XYZ:
        // 全ビルボード
        billboardMatrix_ = cameraMatrix;

        break;

    case BillboardType::Y: {
        // Y軸ビルボード
        float angleY     = std::atan2(toCamera.x, toCamera.z);
        billboardMatrix_ = MakeRotateYMatrix(angleY);

        break;
    }

    default:
        // 何もしない
        billboardMatrix_ = MakeIdentity4x4();
        break;
    }

    if (!adaptRotate.isX) {
        rotation_.x = 0.0f;
    }
    if (!adaptRotate.isY) {
        rotation_.y = 0.0f;
    }
    if (!adaptRotate.isZ) {
        rotation_.z = 0.0f;
    }

    // X/Z軸の回転を適用
    Matrix4x4 xzRotationMatrix = MakeRotateMatrix(rotation_);

    // ビルボード行列計算
    billboardMatrix_ = xzRotationMatrix * billboardMatrix_;

    // 平行移動成分をクリア
    billboardMatrix_.m[3][0] = 0.0f;
    billboardMatrix_.m[3][1] = 0.0f;
    billboardMatrix_.m[3][2] = 0.0f;

    // ワールド行列を計算
    matWorld_ = scaleMatrix * billboardMatrix_ * translateMatrix;

    // parent update
    if (parent_) {
        matWorld_ *= parent_->matWorld_;
    }

    // 定数バッファに転送する
    TransferMatrix();
}

void WorldTransform::SetParent(const WorldTransform* parent) {
    parent_ = parent;
}

Vector3 WorldTransform::LookAt(const Vector3& direction) const {
    // 引数の方向ベクトルを正規化
    Vector3 normalizedDirection = Vector3::Normalize(direction);

    // 現在の行列を基にワールド方向を計算
    Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation_);
    Vector3 worldDirection = TransformNormal(normalizedDirection, rotateMatrix);

    // 正規化して返す
    return Vector3::Normalize(worldDirection);
}

///=====================================================
/// ローカル座標取得
///=====================================================
Vector3 WorldTransform::GetLocalPos() const {
    //
    if (parent_ == nullptr) {
        return GetWorldPos();
    }

    // 親のワールド行列の逆行列を計算
    Matrix4x4 parentInverse = Inverse(parent_->matWorld_);

    // ワールド座標を取得
    Vector3 worldPos = GetWorldPos();

    // 親の逆行列を使ってローカル座標を計算
    Vector3 localPos = TransformMatrix(worldPos, parentInverse);
    return localPos;
}

void WorldTransform::UpdateAffineMatrix() {
    switch (rotateOder_) {
    case RotateOder::XYZ:
        matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
        break;
    case RotateOder::Quaternion:
        quaternion_.Normalize();
        matWorld_ = MakeAffineMatrixQuaternion(scale_, quaternion_, translation_);

        break;
    default:
        break;
    }
}

void WorldTransform::SetParentJoint(const ModelAnimation* animation, const std::string& jointName) {
    if (!animation || jointName.empty()) {
        ClearParentJoint();
        return;
    }

    const auto& skeleton = animation->GetSkeleton();

    // Joint名からインデックスを検索
    auto it = skeleton.jointMap.find(jointName);
    if (it == skeleton.jointMap.end()) {
        ClearParentJoint();
        return;
    }

    parentAnimation_  = animation;
    parentJointIndex_ = it->second;
    parentJointName_  = skeleton.joints[parentJointIndex_].name;

    parent_ = nullptr;
}

///=====================================================
/// WorldPos取得
///=====================================================
Vector3 WorldTransform::GetWorldPos() const {

    return Vector3(
        matWorld_.m[3][0], // X成分
        matWorld_.m[3][1], // Y成分
        matWorld_.m[3][2] // Z成分
    );
}

void WorldTransform::ClearParentJoint() {
    parentAnimation_  = nullptr;
    parentJointIndex_ = -1;
    parentJointName_.clear();

    // 行列を更新
    UpdateMatrix();
}

// Joint親子関係を考慮した行列更新
void WorldTransform::UpdateMatrixWithJoint() {
    if (!HasParentJoint()) {
        return;
    }

    const auto& skeleton = parentAnimation_->GetSkeleton();

    //
    if (parentJointIndex_ < 0 || parentJointIndex_ >= static_cast<int32_t>(skeleton.joints.size())) {

        return;
    }

    // 親JointのskeletonSpaceMatrix取得
    const Joint& parentJoint    = skeleton.joints[parentJointIndex_];
    Matrix4x4 parentJointMatrix = parentJoint.skeletonSpaceMatrix;

    // WorldTransform
    Matrix4x4 animationWorldMatrix = parentAnimation_->GetWorldTransform().matWorld_;
    parentJointMatrix              = parentJointMatrix * animationWorldMatrix;

    matWorld_ *= parentJointMatrix;
}


bool WorldTransform::HasParentJoint() const {
    return parentAnimation_ != nullptr && parentJointIndex_ != -1;
}