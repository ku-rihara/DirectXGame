#include "Player.h"

#include"JoyState/JoyState.h"
#include"Frame/Frame.h"

/// math
#include"MathFunction.h"

#include"Field/Field.h"

/// behavior
#include"PlayerBehavior/PlayerRoot.h"
#include"PlayerBehavior/PlayerJump.h"

#ifdef _DEBUG
#include"imgui.h"
#endif 

float Player::InitY_ = 0.5f;

Player::Player() {}

//初期化
void Player::Init() {

	// 基底クラスの初期化
	BaseObject::Init();
	/// モデルセット
	BaseObject::CreateModel("Player/Player",".obj");

	transform_.translation_.y = Player::InitY_;//  パーツの変位

	jumpSpeed_ = 0.0f;
	muzzelJumpSpeed_ = 1.5f;

	/// 通常モードから
	ChangeBehavior(std::make_unique<PlayerRoot>(this));
}

/// 更新
void Player::Update() {
	prePos_ = GetWorldPosition();
	
	/// ダメージエフェクト
	DamageRendition();
	/// 振る舞い処理
	behavior_->Update();
	//　移動制限
	MoveToLimit();
	// 落ちる
	Fall();

	/// 更新
	BaseObject::Update();
}

void Player::Fall() {
	if (!dynamic_cast<PlayerJump*>(behavior_.get())) {
	
		// 移動
		transform_.translation_.y += fallSpeed_;
		// 重力加速度
		const float kGravityAcceleration = 3.4f * Frame::DeltaTime();
		// 加速度ベクトル
		float accelerationY = -kGravityAcceleration;
		// 加速する
		fallSpeed_ = max(fallSpeed_ + accelerationY, -0.75f);

		// 着地
		if (transform_.translation_.y <= Player::InitY_) {
			transform_.translation_.y = Player::InitY_;
		}
	}
}


/// 描画
void Player::Draw(const ViewProjection& viewProjection) {

	BaseObject::Draw(viewProjection);
}

void Player::DamageRendition() {
	//if (isDamage_) {
	//	damageTime_ -= Frame::DeltaTime();

	//	// ダメージ時間がまだ残っている場合

	//	blinkTimer_ += Frame::DeltaTime();

	//	// チカチカ間隔ごとに透明フラグを切り替え
	//	if (blinkTimer_ >= blinkInterval_) {
	//		blinkTimer_ = 0.0f;          // チカチカタイマーをリセット
	//		isTransparent_ = !isTransparent_; // 透明フラグを反転

	//		if (isTransparent_) {
	//			objColor_.SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f)); // 透明色
	//		}
	//		else {
	//			objColor_.SetColor(Vector4(0.5f, 0.0f, 0.0f, 0.99f)); // 赤常色
	//		}
	//	}

	//	if (damageTime_ <= 0.0f) {
	//		// ダメージ時間が終了したらフラグをリセット
	//		isDamage_ = false;
	//		isTransparent_ = false;
	//		objColor_.SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // 通常色に戻す
	//	}
	//}
}



/// ===================================================
///  Player Move
/// ===================================================

/// 入力処理
Vector3 Player::GetInputVelocity() {
	Vector3 velocity = { 0.0f, 0.0f, 0.0f };
	Input* input = Input::GetInstance();

	// キーボード入力
	if (input->PushKey(DIK_W)) {
		velocity.z += 1.0f;  // 前進
	}
	if (input->PushKey(DIK_S)) {
		velocity.z -= 1.0f;  // 後退
	}
	if (input->PushKey(DIK_A)) {
		velocity.x -= 1.0f;  // 左移動
	}
	if (input->PushKey(DIK_D)) {
		velocity.x += 1.0f;  // 右移動
	}

	// ジョイスティック入力
	XINPUT_STATE joyState;
	if (input->GetJoystickState(0, joyState)) {
		velocity.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX;
		velocity.z += (float)joyState.Gamepad.sThumbLY / SHRT_MAX;
	}

	return velocity;
}

// 移動の処理
void Player::Move(const float& speed) {

	/// Inuputから速度代入
	velocity_ = GetInputVelocity();

	/// 移動処理
	if (GetIsMoving()) {
		// 移動ベクトルの正規化と速さの適用
		velocity_ = (velocity_).Normalize() * speed;

		// 移動ベクトルをカメラの角度に基づいて回転
		Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_->rotation_.y);
		velocity_ = TransformNormal(velocity_, rotateMatrix);

		// 位置を更新
		transform_.translation_ += velocity_;

		// 目標角度を計算
		objectiveAngle_ = std::atan2(velocity_.x, velocity_.z);

		// 最短角度補間でプレイヤーの回転を更新
		transform_.rotation_.y = LerpShortAngle(transform_.rotation_.y, objectiveAngle_, 0.3f);
	}
}

//動いているか
bool Player::GetIsMoving() {
	Input* input = Input::GetInstance();
	bool isMoving = false;
	const float thresholdValue = 0.3f;
	Vector3 StickVelocity;
	Vector3 keyBoradVelocity;
	/// ===================================================
	///  JoyStick
	/// ===================================================
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 移動量
		StickVelocity = { (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0, (float)joyState.Gamepad.sThumbLY / SHRT_MAX };
		if (Vector3::Length((StickVelocity)) > thresholdValue) {
			isMoving = true;
		}
	}
	/// ===================================================
	///  keyBorad
	/// ===================================================
	else {
		// キーボード入力
		if (input->PushKey(DIK_W)) {
			keyBoradVelocity.z += 1.0f;  // 前進
		}
		if (input->PushKey(DIK_S)) {
			keyBoradVelocity.z -= 1.0f;  // 後退
		}
		if (input->PushKey(DIK_A)) {
			keyBoradVelocity.x -= 1.0f;  // 左移動
		}
		if (input->PushKey(DIK_D)) {
			keyBoradVelocity.x += 1.0f;  // 右移動
		}
		if (Vector3::Length((keyBoradVelocity)) > 0) {
			isMoving = true;
		}
	}
	return isMoving;
}

/// ===================================================
///  Player Jump
/// ===================================================

void Player::Jump(float& speed) {
	// 移動
	transform_.translation_.y += speed;
	// 重力加速度
	const float kGravityAcceleration = 4.4f * Frame::DeltaTime();
	// 加速度ベクトル
	float accelerationY = -kGravityAcceleration;
	// 加速する
	speed = max(speed + accelerationY, jumpLimit_);

	// 着地
	if (transform_.translation_.y <= Player::InitY_) {
		transform_.translation_.y = Player::InitY_;

		// ジャンプ終了
		ChangeBehavior(std::make_unique<PlayerRoot>(this));
	}
}

void Player::MoveToLimit() {

	// フィールドの中心とスケールを取得
	Vector3 fieldCenter = { 0.0f, 0.0f, 0.0f }; // フィールド中心 
	Vector3 fieldScale = Field::baseScale_;     // フィールドのスケール

	// プレイヤーのスケールを考慮した半径
	float radiusX = fieldScale.x - transform_.scale_.x;
	float radiusZ = fieldScale.z - transform_.scale_.z;

	// 現在位置が範囲内かチェック
	bool insideX = std::abs(transform_.translation_.x - fieldCenter.x) <= radiusX;
	bool insideZ = std::abs(transform_.translation_.z - fieldCenter.z) <= radiusZ;

	//////////////////////////////////////////////////////
	///範囲外なら戻す
	//////////////////////////////////////////////////////

	if (!insideX) {/// X座標
		transform_.translation_.x = std::clamp(
			transform_.translation_.x,
			fieldCenter.x - radiusX,
			fieldCenter.x + radiusX
		);
	}

	if (!insideZ) {/// Z座標
		transform_.translation_.z = std::clamp(
			transform_.translation_.z,
			fieldCenter.z - radiusZ,
			fieldCenter.z + radiusZ
		);
	}

	// 範囲外の反発処理
	if (!insideX || !insideZ) {
		Vector3 directionToCenter = (fieldCenter - transform_.translation_).Normalize();
		transform_.translation_.x += directionToCenter.x * 0.1f; // 軽く押し戻す
		transform_.translation_.z += directionToCenter.z * 0.1f; // 軽く押し戻す
	}
}

void Player::ChangeBehavior(std::unique_ptr<BasePlayerBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}



void Player::Debug() {
#ifdef _DEBUG
	if (ImGui::TreeNode("Player")) {
		ImGui::DragFloat3("Pos", &transform_.translation_.x, 0.01f);
		ImGui::DragFloat("JumpSpeed", &muzzelJumpSpeed_, 0.01f);
		ImGui::Text("Isattack:%d", isAttack_);
		behavior_->Debug();
		ImGui::TreePop();
	}
#endif // _DEBUG
}

//Vector3 Player::GetCenterPosition() const {
//
//	const Vector3 offset = { 0.0f, 1.0f, 0.0f };//ローカル座標のオフセット
//	// ワールド座標に変換
//	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
//
//	return worldPos;
//};

void Player::TakeDamage() {
	/*if (!isDamage_) {
		life_--;
		isDamage_ = true;
		damageTime_ = damageCollTime_;
	}*/
}

///=========================================================
/// Class Set
///==========================================================

void Player::SetBoss(Boss* boss) {
	pBoss_ = boss;
}

///=========================================================
/// Collision
///==========================================================
