#include "PlayerBullet.h"
#include "TextureManager.h"
#include"assert.h"

void PlayerBullet::Init(const Vector3& position, const Vector3& velocity) {
	
	object3D_.reset(Object3d::CreateModel("CollisionSphere", ".obj"));
	velocity_ = velocity;
	//引数で受け取った初期座標をセット
	object3D_->transform_.translation_ = position;
}

void PlayerBullet::Update() {
	//座標を移動させる
	object3D_->transform_.translation_ += velocity_;
	if (--deathTimer_ <= 0) {
		isDeath_ = true;
	}
	object3D_->Update();
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	//モデルの描画
	object3D_->Draw(viewProjection, textureHandle_);
}

Vector3 PlayerBullet::GetWorldPos() {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3D_->transform_.matWorld_.m[3][0];
	worldPos.y = object3D_->transform_.matWorld_.m[3][1];
	worldPos.z = object3D_->transform_.matWorld_.m[3][2];

	return worldPos;
}

void PlayerBullet::OnColligion() { isDeath_ = true; }