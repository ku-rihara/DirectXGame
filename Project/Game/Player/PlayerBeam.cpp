#include "PlayerBeam.h"
#include "base/TextureManager.h"
#include "assert.h"
#include <cmath>

void PlayerBeam::Init() {
    object3D_.reset(Object3d::CreateModel("PlayerBeam",".obj"));
}

void PlayerBeam::Update(const Vector3& position, const Vector3& direction) {
    velocity_ = direction;

    // 位置を設定
    transform_.translation_ = position;

    // 方向ベクトルを基にX軸とY軸の回転を計算
    float rotateY = std::atan2(direction.x, direction.z);
    float rotateX = std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z));

    // 回転を設定
    transform_.rotation_ = { rotateX, rotateY, 0.0f };

    transform_.UpdateMatrix();
}

void PlayerBeam::Draw(const ViewProjection& viewProjection) {
    // モデルの描画
    object3D_->Draw(transform_, viewProjection, textureHandle_);
}

Vector3 PlayerBeam::GetWorldPos() {
    Vector3 worldPos;
    // ワールド行列の平行移動成分を取得
    worldPos.x = transform_.matWorld_.m[3][0];
    worldPos.y = transform_.matWorld_.m[3][1];
    worldPos.z = transform_.matWorld_.m[3][2];

    return worldPos;
}

Vector3 PlayerBeam::GetBaseCenterPosition() const {
    Vector3 worldPos;
    // ワールド行列の平行移動成分を取得
    worldPos.x = transform_.matWorld_.m[3][0];
    worldPos.y = transform_.matWorld_.m[3][1];
    worldPos.z = transform_.matWorld_.m[3][2];

    return worldPos;
}
