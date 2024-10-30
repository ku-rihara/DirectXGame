#include "PlayerBullet.h"
#include "base/TextureManager.h"
#include"assert.h"

void PlayerBullet::Init(const Vector3& position, const Vector3& velocity) {
	
	object3D_.reset(Object3d::CreateModel("CollisionSphere", ".obj"));
	velocity_ = velocity;
	//引数で受け取った初期座標をセット
	transform_.translation_ = position;
}

void PlayerBullet::Update() {
	//座標を移動させる
transform_.translation_ += velocity_;
	if (--deathTimer_ <= 0) {
		isDeath_ = true;
	}
	transform_.UpdateMatrix();
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	//モデルの描画
	object3D_->Draw(transform_,viewProjection, textureHandle_);
}

Vector3 PlayerBullet::GetWorldPos() {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = transform_.matWorld_.m[3][0];
	worldPos.y = transform_.matWorld_.m[3][1];
	worldPos.z = transform_.matWorld_.m[3][2];

	return worldPos;
}

Vector3 PlayerBullet::GetBaseCenterPosition() const {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = transform_.matWorld_.m[3][0];
	worldPos.y = transform_.matWorld_.m[3][1];
	worldPos.z = transform_.matWorld_.m[3][2];

	return worldPos;
}

//void BaseEnemy::OnCollisionEnter([[maybe_unused]] Colider* other) {
//	if (dynamic_cast<PlayerBullet*>(other)) {
//		isDeath_ = true;
//	}
//}