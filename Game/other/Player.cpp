#include "Player.h"
#include "Easing.h"
#include "MathFunction.h"
#include "Matrix4x4.h"
#include "cassert"
#include <imgui.h>
// class
#include "CollisionTypeIdDef.h"
#include "Hummer.h"
#include "LockOn.h"
#include "Pi.h"
Player::Player() {}

void Player::Init(const std::vector<Model*>& models) {

	partsWorldTransforms_.reserve(partsnum); // メモリを確保
	for (int i = 0; i < partsnum; ++i) {
		partsWorldTransforms_.emplace_back(std::make_unique<WorldTransform>());
	}
	// 基底クラスの初期化
	BaseCharacter::Init(models);
	// パーツの親子関係
	partsWorldTransforms_[IndexBody]->parent_ = &baseWorldTransform_;
	partsWorldTransforms_[IndexHead]->parent_ = partsWorldTransforms_[IndexBody].get();
	partsWorldTransforms_[IndexLeftArm]->parent_ = partsWorldTransforms_[IndexBody].get();
	partsWorldTransforms_[IndexRightArm]->parent_ = partsWorldTransforms_[IndexBody].get();
	////  ハンマー初期化
	if (hummer_ != nullptr) {
		hummer_->SetParent(*partsWorldTransforms_[IndexBody].get());
	}
	/*hummer_->SetRadius(0.0f);*/

	//  パーツの変位の値
	baseWorldTransform_.translation_.y = 0.9f;

	BehaviorRootInitialize();
	globalParameter_ = GlobalParameter::GetInstance();
	const char* groupName = "Player";
	// グループを追加
	globalParameter_->CreateGroup(groupName);
	/*globalParameter_->AddItem(groupName, "Test", 90);*/
	globalParameter_->AddItem(groupName, "Head Translation", partsWorldTransforms_[IndexHead]->translation_);
	globalParameter_->AddItem(groupName, "ArmL Translation", partsWorldTransforms_[IndexLeftArm]->translation_);
	globalParameter_->AddItem(groupName, "ArmR Translation", partsWorldTransforms_[IndexRightArm]->translation_);
	globalParameter_->AddItem(groupName, "floatingCycle", floatingCycle_);
	globalParameter_->AddItem(groupName, "floatingAmplitude", floatingAmplitude_);

	globalParameter_->AddItem(groupName, "EaseMaxTime", easeTime);
	globalParameter_->AddItem(groupName, "amplitude", amplitude);
	globalParameter_->AddItem(groupName, "period", period);
}

void Player::Update() {
	ApplyGlobalParameter();
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;
		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;
		case Behavior::kScaling:
			BehabiorScalinginitialize();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
	// 振る舞い更新
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;

	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kDash:
		BehabiorDashUpdate();
		break;
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	case Behavior::kScaling:
		BehabiorScalingUpdate();
		break;
	}
	if (Input::GetInstance()->TrrigerKey(DIK_L)) {
		behaviorRequest_ = Behavior::kScaling;
	}
	partsWorldTransforms_[IndexBody]->scale_ = EaseAmplitudeScale(Vector3(1.0f, 1.0f, 1.0f), easeT, easeTime, amplitude, period);
	hummer_->Update();
	BaseCharacter::Update();
}
void Player::Draw(const ViewProjection& viewProjection) {
	hummer_->Draw(viewProjection);
	BaseCharacter::Draw(viewProjection);
}
/*関数*/
void Player::AnimationUpdate() {

	// 1フレームでのパラメータ加算値
	const float step = 2.0f * float(pi) / floatingCycle_;
	// パラメータを1ステップ分加算
	floatingParameter_ += step;
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * pi);
	// 浮遊の振幅＜m＞
	/*floatingAmplitude_ = 0.2f;*/
	// 浮遊を座標に反映
	partsWorldTransforms_[IndexBody]->translation_.y = std::sin(floatingParameter_) * floatingAmplitude_;

	/*ImGui::Begin("Playe");
	ImGui::SliderFloat3("Head Translation", &partsWorldTransforms_[IndexHead]->translation_.x, 0, 2.0f);
	ImGui::SliderFloat3("ArmL Translation", &partsWorldTransforms_[IndexLeftArm]->translation_.x, 0, 2.0f);
	ImGui::SliderFloat3("ArmR Translation", &partsWorldTransforms_[IndexRightArm]->translation_.x, -1, 2.0f);
	ImGui::End();*/
}

void Player::BehaviorRootUpdate() {

	Move(0.3f);

	// Rで攻撃
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		behaviorRequest_ = Behavior::kAttack;
	}
	// Lでダッシュ
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
		behaviorRequest_ = Behavior::kDash;
	}
	// 一旦Jでジャンプ
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		behaviorRequest_ = Behavior::kJump;
	}
}

void Player::BehaviorAttackUpdate() {
	if (lockOn_ && lockOn_->GetEnemyTarget()) {
		Vector3 differectialVector = lockOn_->GetTargetPosition() - GetBaseCenterPosition();
		// 距離
		float distance = Length(differectialVector);
		// 距離しきい値
		const float threshold = 0.2f;
		// しきい値より離れている時
		if (distance > threshold) {
			// Y軸回り角度
			baseWorldTransform_.rotation_.y = std::atan2(differectialVector.x, differectialVector.z);
			// しきい値を超える速さなら補正する
			if (attackSpeed > distance - threshold) {
				attackSpeed = distance - threshold;
			}
		}
	}
	/*浮遊移動のサイクル*/
	AttackEaseT_ += 0.05f;
	attackMoveT_ += 0.1f;
	if (attackMoveT_ >= 1.0f) {
		attackMoveT_ = 1.0f;
	}
	if (AttackEaseT_ >= 1.0f) {
		AttackEaseT_ = 1.0f;
		stiffeningTime_++;
		if (stiffeningTime_ >= 10) {
			behaviorRequest_ = Behavior::kRoot;
		}
	}
	// 攻撃先の座標を決める
	Vector3 attackPos = attackDirection_ * attackSpeed;

	baseWorldTransform_.translation_ = Lerp(baseWorldTransform_.translation_, savePos_ + attackPos, attackMoveT_);
	// 回転する
	hummer_->Attack(AttackEaseT_);
	partsWorldTransforms_[IndexRightArm]->rotation_.x = Lerp(2.4f, 5.0f, AttackEaseT_);
	partsWorldTransforms_[IndexLeftArm]->rotation_.x = Lerp(2.4f, 5.0f, AttackEaseT_);
}
// ダッシュ更新
void Player::BehabiorDashUpdate() {
	Move(2.7f);
	// ダッシュの時間
	const uint32_t behaviorDashTime = 120;

	// 既定の時間経過で通常行動に戻る
	if (++workDash_.dashPrameter_ >= behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorJumpUpdate() {
	// 移動
	baseWorldTransform_.translation_ += velocity_;
	// 重力加速度
	const float kGravityAcceleration = 0.05f;
	// 加速度ベクトル
	Vector3 accelerationVector = { 0, -kGravityAcceleration, 0 };
	// 加速する
	velocity_ += accelerationVector;

	// 着地
	if (baseWorldTransform_.translation_.y <= 0.0f) {
		baseWorldTransform_.translation_.y = 0.9f;
		// ジャンプ終了
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehabiorScalingUpdate() {
	easeT += 0.01f;
	if (easeT >= easeTime) {
		easeT = easeTime;
		// ジャンプ終了
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::Move(const float& speed) {
	AnimationUpdate();
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 速さ
		const float thresholdValue = 0.7f;
		bool isMoving = false;
		// 移動量
		velocity_ = { (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0, (float)joyState.Gamepad.sThumbLY / SHRT_MAX };
		if (Length(velocity_) > thresholdValue) {
			isMoving = true;
		}
		if (isMoving) {
			// 移動量に速さを反映
			velocity_ = Normalize(velocity_) * speed;
			// 移動ベクトルをカメラの角度だけ回転する
			Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_->rotation_.y);
			velocity_ = TransformNormal(velocity_, rotateMatrix);
			// 移動
			baseWorldTransform_.translation_ += velocity_;
			// 目標角度
			objectiveAngle_ = std::atan2(velocity_.x, velocity_.z);
			// 最短角度補間
			baseWorldTransform_.rotation_.y = LerpShortAngle(baseWorldTransform_.rotation_.y, objectiveAngle_, 0.3f);

		}
		else if (lockOn_ && lockOn_->GetEnemyTarget()) {
			Vector3 differectialVector = lockOn_->GetTargetPosition() - GetBaseCenterPosition();

			// Y軸周り角度(θy)
			baseWorldTransform_.rotation_.y = std::atan2(differectialVector.x, differectialVector.z);
		}
	}
}

// 通常初期化
void Player::BehaviorRootInitialize() {
	hummer_->HistoryClear();
	hummer_->SetScale(Vector3{ 0, 0, 0 });
	hummer_->SetRadius(0.0f);
	/*hummer_->SetRadius(0.0f);*/
	partsWorldTransforms_[IndexLeftArm]->rotation_ = { 0, 0, 0 };
	partsWorldTransforms_[IndexRightArm]->rotation_ = { 0, 0, 0 };
	AnimationInit();
}
// アタック初期化
void Player::BehaviorAttackInitialize() {
	hummer_->SetScale(Vector3{ 1, 1, 1 });
	hummer_->SetRadius(3.0f);
	stiffeningTime_ = 0;
	AttackEaseT_ = 0;
	attackMoveT_ = 0;

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(baseWorldTransform_.rotation_.y);
	Vector3 forward = { 0, 0, 1 };
	Vector3 direction = TransformNormal(forward, rotateMatrix);
	savePos_ = baseWorldTransform_.translation_;
	attackDirection_ = Normalize(direction);
	attackSpeed = 5.0f;
}
// ダッシュ初期化
void Player::BehaviorDashInitialize() {
	workDash_.dashPrameter_ = 0;
	baseWorldTransform_.rotation_.y = objectiveAngle_;
}

void Player::BehaviorJumpInitialize() {
	partsWorldTransforms_[IndexBody]->translation_.y = 0;
	partsWorldTransforms_[IndexLeftArm]->rotation_.x = 0;
	partsWorldTransforms_[IndexRightArm]->rotation_.x = 0;
	// ジャンプ初速
	const float kJumpFirstSpeed = 1.0f;
	// ジャンプ初速を与える
	velocity_.y = kJumpFirstSpeed;
}
void Player::BehabiorScalinginitialize() { easeT = 0; }

// アニメーション初期化
void Player::AnimationInit() { floatingParameter_ = 0.0f; }

Vector3 Player::GetBaseCenterPosition() const {
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 1.5f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transform(offset, baseWorldTransform_.matWorld_);
	return worldPos;
}

void Player::ApplyGlobalParameter() {

	GlobalParameter* globalParameter = GlobalParameter::GetInstance();
	const char* groupName = "Player";
	partsWorldTransforms_[IndexHead]->translation_ = globalParameter->GetValue<Vector3>(groupName, "Head Translation");
	partsWorldTransforms_[IndexLeftArm]->translation_ = globalParameter->GetValue<Vector3>(groupName, "ArmL Translation");
	partsWorldTransforms_[IndexRightArm]->translation_ = globalParameter->GetValue<Vector3>(groupName, "ArmR Translation");
	floatingCycle_ = globalParameter->GetValue<int32_t>(groupName, "floatingCycle");
	floatingAmplitude_ = globalParameter->GetValue<float>(groupName, "floatingAmplitude");

	/*easeT = globalParameter->GetValue<float>(groupName, "easeT");*/
	easeTime = globalParameter->GetValue<float>(groupName, "easeTime");
	amplitude = globalParameter->GetValue<float>(groupName, "amplitude");
	period = globalParameter->GetValue<float>(groupName, "period");
}

void Player::OnCollision([[maybe_unused]] Colider* other) {
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {
		// ジャンプリクエスト
		behaviorRequest_ = Behavior::kJump;
	}
}