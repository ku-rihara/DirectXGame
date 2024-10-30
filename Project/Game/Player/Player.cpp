#include"Player.h"
#include"3d/ModelManager.h"
//class
#include"Reticle/Reticle.h"
//input
#include"input/Input.h"
/// frame
#include"Frame/Frame.h"

void  Player::Init() {
	/// プレイヤーのモデル
	obj3D_.reset(Object3d::CreateModel("suzanne", ".obj"));
}
/// 更新
void  Player::Update() {

	/// 弾発射
	BulletShot();

	// ですフラグの立った弾を削除
	for (auto it = bullets_.begin(); it != bullets_.end(); ) {
		if ((*it)->GetIsDead()) {
			it = bullets_.erase(it);
		}
		else {
			++it;
		}
	}

	// 弾更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
	transform_.UpdateMatrix();
}
/// 描画
void  Player::Draw(const ViewProjection& viewProjection) {
	obj3D_->Draw(transform_, viewProjection);
	
}

/// 弾の描画
void  Player::BulletDraw(const ViewProjection& viewProjection) {
	
	for (auto& bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Player::BulletShot() {
	if (Input::IsTriggerMouse(0)) {
		// 弾の速度
		const float kBulletSpeed = 65.0f*Frame::DeltaTime();

		// 自機から標準オブジェクトへのベクトル
		Vector3 velocity = pReticle_->GetWorld3DRecticlPos() - GetWorldPos();
		velocity = Vector3::Normalize(velocity) * kBulletSpeed;
		// 弾を生成
		std::unique_ptr<PlayerBullet> newBullet;
		newBullet = std::make_unique<PlayerBullet>();
		/// 初期化
		newBullet->Init(GetWorldPos(), velocity);
		// 弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}


Vector3 Player::GetWorldPos() {
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得
	worldPos.x = transform_.matWorld_.m[3][0];
	worldPos.y = transform_.matWorld_.m[3][1];
	worldPos.z = transform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetReticle(Reticle* reticle) {
	pReticle_ = reticle;
}

//setter
//親子関係を結ぶ
void Player::SetParent(const WorldTransform* parent) { transform_.parent_ = parent; }