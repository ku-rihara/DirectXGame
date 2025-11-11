#include "WorldTransform.h"
#include "Animation/Object3DAnimation.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"

WorldTransform::WorldTransform()  = default;
WorldTransform::~WorldTransform() = default;

void WorldTransform::Init() {

    scale_       = Vector3::OneVector();
    rotation_    = {};
    translation_ = {};

    //  行列の更新
    UpdateMatrix();

    // オブジェクトイージングアニメーションプレイヤー初期化
    if (!objEaseAnimationPlayer_) {
        objEaseAnimationPlayer_ = std::make_unique<ObjEaseAnimationPlayer>();
        objEaseAnimationPlayer_->Init();
    }
}

void WorldTransform::TransferMatrix() {
}

void WorldTransform::UpdateMatrix() {
    // SRT更新
    UpdateAffineMatrix();

    // JointParent
    if (HasParentJoint()) {
        UpdateMatrixWithJoint();
    }
    // 通常のParent
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

    if (HasParentJoint()) {
        UpdateMatrixWithJoint();
    }

    else if (parent_) {
        // 親の行列から回転成分だけ取得
        Matrix4x4 parentRotationOnly = parent_->matWorld_;
        parentRotationOnly.m[3][0]   = 0.0f;
        parentRotationOnly.m[3][1]   = 0.0f;
        parentRotationOnly.m[3][2]   = 0.0f;

        // 親の位置成分を取得
        Vector3 parentPosition = parent_->GetWorldPos();

        // 現在のワールド行列に親の回転のみ適用
        Matrix4x4 tempMatrix = matWorld_;

        // 現在の位置を一旦保存
        Vector3 currentPos = Vector3(tempMatrix.m[3][0], tempMatrix.m[3][1], tempMatrix.m[3][2]);

        // 親の回転を現在の位置に適用
        Vector3 rotatedOffset = TransformNormal(currentPos, parentRotationOnly);

        matWorld_.m[3][0] = parentPosition.x + rotatedOffset.x;
        matWorld_.m[3][1] = parentPosition.y + rotatedOffset.y;
        matWorld_.m[3][2] = parentPosition.z + rotatedOffset.z;
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

void WorldTransform::SetParentJoint(const Object3DAnimation* animation, const std::string& jointName) {
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

void WorldTransform::UpdateMatrixWithJoint() {
    if (!HasParentJoint()) {
        return;
    }

    const auto& skeleton = parentAnimation_->GetSkeleton();

    //
    if (parentJointIndex_ < 0 || parentJointIndex_ >= static_cast<int32_t>(skeleton.joints.size())) {

        return;
    }

    // 親JointSkeletonSpaceMatrix取得
    const Joint& parentJoint    = skeleton.joints[parentJointIndex_];
    Matrix4x4 parentJointMatrix = parentJoint.skeletonSpaceMatrix;

    // WorldTransform
    Matrix4x4 animationWorldMatrix = parentAnimation_->transform_.matWorld_;
    parentJointMatrix              = parentJointMatrix * animationWorldMatrix;

    matWorld_ *= parentJointMatrix;
}

bool WorldTransform::HasParentJoint() const {
    return parentAnimation_ != nullptr && parentJointIndex_ != -1;
}

Vector3 WorldTransform::GetRightVector() const {

    if (rotateOder_ == RotateOder::Quaternion) {
        Matrix4x4 rotationMatrix = quaternion_.MakeRotateMatrix();
        return TransformNormal(Vector3::ToRight(), rotationMatrix).Normalize();
    }

    return Vector3(matWorld_.m[0][0], matWorld_.m[1][0], matWorld_.m[2][0]);
}

Vector3 WorldTransform::GetUpVector() const {

    if (rotateOder_ == RotateOder::Quaternion) {
        Matrix4x4 rotationMatrix = quaternion_.MakeRotateMatrix();
        return TransformNormal(Vector3::ToUp(), rotationMatrix).Normalize();
    }

    return Vector3(matWorld_.m[0][1], matWorld_.m[1][1], matWorld_.m[2][1]);
}

Vector3 WorldTransform::GetForwardVector() const {

    if (rotateOder_ == RotateOder::Quaternion) {
        Matrix4x4 rotationMatrix = quaternion_.MakeRotateMatrix();
        return TransformNormal(Vector3::ToForward(), rotationMatrix).Normalize();
    }

    return Vector3(matWorld_.m[0][2], matWorld_.m[1][2], matWorld_.m[2][2]);
}

Vector3 WorldTransform::CalcForwardTargetPos(const Vector3& startPos, const Vector3& offsetValue) const {
    // 移動イージングの初期化
    const Vector3& startPosition = startPos;

    // 向き(Y軸回転)を取得
    float playerRotationY    = rotation_.y;
    Matrix4x4 rotationMatrix = MakeRotateYMatrix(playerRotationY);

    // ローカル座標での移動ベクトルをワールド座標に変換
    Vector3 localMoveVector = offsetValue;
    Vector3 worldMoveVector = TransformNormal(localMoveVector, rotationMatrix);

    // 目標位置を計算
    return startPosition + worldMoveVector;
}

///============================================================
/// オブジェクトイージングアニメーション再生
///============================================================
void WorldTransform::PlayObjEaseAnimation(const std::string& categoryName, const std::string& animationName) {
    if (!objEaseAnimationPlayer_) {
        objEaseAnimationPlayer_ = std::make_unique<ObjEaseAnimationPlayer>();
        objEaseAnimationPlayer_->Init();
    }

    objEaseAnimationPlayer_->Play(categoryName, animationName);

    // Rail使用時、親を設定
    if (objEaseAnimationPlayer_->GetAnimationData() && objEaseAnimationPlayer_->GetAnimationData()->IsUsingRail()) {
        auto* railPlayer = objEaseAnimationPlayer_->GetAnimationData()->GetRailPlayer();
        if (railPlayer) {
            railPlayer->SetParent(this);
        }
    }
}

///============================================================
/// オブジェクトイージングアニメーション停止
///============================================================
void WorldTransform::StopObjEaseAnimation() {
    if (objEaseAnimationPlayer_) {
        objEaseAnimationPlayer_->Stop();
    }
}

///============================================================
/// アニメーション更新
///============================================================
void WorldTransform::UpdateObjEaseAnimation(const float& deltaTime) {
    if (objEaseAnimationPlayer_) {
        objEaseAnimationPlayer_->Update(deltaTime);
        ApplyAnimationToTransform();
    }
}

///============================================================
/// アニメーション適用後のTransform更新
///============================================================
void WorldTransform::ApplyAnimationToTransform() {
    if (!objEaseAnimationPlayer_ || !objEaseAnimationPlayer_->IsPlaying()) {
        return;
    }

    auto* animData = objEaseAnimationPlayer_->GetAnimationData();
    if (!animData) {
        return;
    }

    // Scaleを適用
    scale_ = objEaseAnimationPlayer_->GetCurrentScale();

    // Rotationを適用（ラジアン）
    rotation_ = objEaseAnimationPlayer_->GetCurrentRotation();

    // Translationを適用
    if (!animData->IsUsingRail()) {
        // 通常のイージング使用時
        translation_ = objEaseAnimationPlayer_->GetCurrentTranslation();
    } else {
        // Rail使用時
        auto* railPlayer = animData->GetRailPlayer();
        if (railPlayer) {
            translation_ = railPlayer->GetCurrentPosition();
        }
    }
}