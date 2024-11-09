#include"Player.h"
#include<imgui.h>
//class
#include"Reticle/Reticle.h"
//input
#include"input/Input.h"
/// frame
#include"Frame/Frame.h"

void  Player::Init() {
	/// プレイヤーのモデル
	obj3D_.reset(Object3d::CreateModel("suzanne", ".obj"));
	/// ビームの初期化
	beam_ = std::make_unique<PlayerBeam>();
	/// 初期化
	beam_->Init();
	//beam_->SetParent(transform_);

}
/// 更新
void  Player::Update() {

	/// 弾発射
	BeamShot();
	
	transform_.UpdateMatrix();
}
/// 描画
void  Player::Draw(const ViewProjection& viewProjection) {
	obj3D_->Draw(transform_, viewProjection);

}

/// 弾の描画
void  Player::BulletDraw(const ViewProjection& viewProjection) {

	beam_->Draw(viewProjection);
}

void Player::SpriteDraw() {
	beam_->SpriteDraw();
}

void Player::BeamShot() {
	if (Input::IsPressMouse(0)) {
		
		// 自機から標準オブジェクトへのベクトル
		Vector3 direction = pReticle_->GetWorld3DRecticlPos() - GetWorldPos();
		direction = Vector3::Normalize(direction) /*kBulletSpeed*/;
		beam_->Update(GetWorldPos(),direction);
		beam_->DecreaseGauge();
	}
	else {
		beam_->IncreaseGauge();
	}
}

void Player::Debug() {
	if (ImGui::TreeNode("Player")) {
		ImGui::DragFloat3("pos", &transform_.translation_.x, 0.01f);
		if (ImGui::TreeNode("beam")) {
			beam_->Debug();
			ImGui::TreePop();
		}
		ImGui::TreePop();
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