#include"Player.h"
#include<imgui.h>
//class
#include"Reticle/Reticle.h"
//input
#include"input/Input.h"
#include"JoyState/JoyState.h"

/// math
#include"MathFunction.h"
/// frame
#include"Frame/Frame.h"
#include"Camera/GameCamera.h"
#include"ControlPoint/SlowPoint.h"
#include"ControlPoint/FastPoint.h"
#include"ControlPoint/NormalPoint.h"
#include"ControlPoint/StopPoint.h"


void  Player::Init() {
	/// プレイヤーのモデル
	obj3D_.reset(Object3d::CreateModel("suzanne", ".obj"));
	/// ビームの初期化
	beam_ = std::make_unique<PlayerBeam>();
	/// 初期化
	beam_->Init();

	obj3D_->SetLightMode(2);
	transform_.Init();

	transform_.translation_.z = 0.18f;
	transform_.translation_.y = -1.05f;
	transform_.isNotViewRestriction_ = true;

	targetSpeed_ = 0.15f;
	currentSpeed_ = 0.0f;
	lerpSpeed_ = 1.2f;

	beam_->SetParent(transform_);

}
/// 更新
void  Player::Update() {
	if (isStop_) {
		stopTime_ += Frame::DeltaTime();
		if (stopTime_ >= stopTimeMax_) {
			stopTime_ = 0.0f;
			isStop_ = false;
			targetSpeed_ = 0.15f;  // 通常速度に戻す
		}
	}

	// Lerp関数を使ってcurrentSpeed_をtargetSpeed_に補間
	currentSpeed_ = Lerp(currentSpeed_, targetSpeed_, lerpSpeed_ * Frame::DeltaTime());
	pGameCamera_->SetMoveSpeed(currentSpeed_);

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
	if (Input::IsPressMouse(0) || joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {

		beam_->Draw(viewProjection);
	}
}

void Player::SpriteDraw() {
	beam_->SpriteDraw();
}

void Player::BeamShot() {
	if (Input::IsPressMouse(0) || joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {

		// 自機から標準オブジェクトへのベクトル
		Vector3 direction = pReticle_->GetWorld3DRecticlPos() - GetWorldPos();
		direction = Vector3::Normalize(direction) /*kBulletSpeed*/;
		beam_->Update(pGameCamera_->GetCameraRotate(), direction);
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

	if (ImGui::Button("STOP")) {
		targetSpeed_ = 0.0f;  // 通常速度に戻す
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

void Player::SetGameCamera(GameCamera* gamecamera) {
	pGameCamera_ = gamecamera;
}


Vector3 Player::GetBaseCenterPosition() const {
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };//ローカル座標のオフセット
	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);

	return worldPos;
}

void Player::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	if (dynamic_cast<FastPoint*>(other)) {
		targetSpeed_ = 0.65f;
	}
	else if (dynamic_cast<SlowPoint*>(other)) {
		targetSpeed_ = 0.15f;
	}
	else if (dynamic_cast<NormalPoint*>(other)) {
		targetSpeed_ = 0.3f;
	}
	else if (dynamic_cast<StopPoint*>(other)) {
		targetSpeed_ = 0.0f;
		isStop_ = true;
	}
}