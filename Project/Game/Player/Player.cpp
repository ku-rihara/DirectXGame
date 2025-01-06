#include "Player.h"

/// input
#include"JoyState/JoyState.h"

/// frame
#include"Frame/Frame.h"

/// math
#include"MathFunction.h"

/// object
#include"Field/Field.h"
#include"LockOn/LockOn.h"

/// behavior
#include"PlayerBehavior/PlayerRoot.h"
#include"PlayerBehavior/PlayerJump.h"
#include"ComboAttackBehavior/ComboAttackRoot.h"

/// imgui
#include<imgui.h>

///=========================================================
///　static 変数初期化
///==========================================================
float Player::InitY_ = 0.5f;

Player::Player() {}

///=========================================================
///　初期化
///==========================================================
void Player::Init() {

	///* model生成
	BaseObject::Init();	// 基底クラスの初期化 
	BaseObject::CreateModel("cube",".obj");/// モデルセット

	///* グローバルパラメータ
	globalParameter_ = GlobalParameter::GetInstance();
	globalParameter_->CreateGroup(groupName_, false);
	AddParmGroup();
	ApplyGlobalParameter();


	///* 武器生成
	leftHand_ = std::make_unique<PlayerHandLeft>();
	rightHand_ = std::make_unique<PlayerHandRight>();

	leftHand_->Init();
	rightHand_->Init();

	///* ペアレント
	leftHand_->SetParent(&transform_);
	rightHand_->SetParent(&transform_);

	/// レールペアレント
	rightHand_->SetRailParent(&transform_);
	leftHand_->SetRailParent(&transform_);

	/// 通常モードから
	ChangeBehavior(std::make_unique<PlayerRoot>(this));
	ChangeComboBehavior(std::make_unique<ComboAttackRoot>(this));
}

///=========================================================
///　更新
///==========================================================
void Player::Update() {
	prePos_ = GetWorldPosition();// 前フレームの座標

	DamageRendition();        /// ダメージエフェクト

	/// 振る舞い処理(コンボ攻撃中は中止)
	if (dynamic_cast<ComboAttackRoot*>(comboBehavior_.get())) {
		behavior_->Update();
		
	}

	comboBehavior_->Update();	  ///　コンボ攻撃攻撃
	MoveToLimit();                ///　移動制限
	//Fall();                     /// 落ちる

	/// 行列更新
	leftHand_->Update();
	rightHand_->Update();
	BaseObject::Update();        /// 更新 

}

///=========================================================
///　描画
///==========================================================
void Player::Draw(const ViewProjection& viewProjection) {

	BaseObject::Draw(viewProjection);
	leftHand_->Draw(viewProjection);
	rightHand_->Draw(viewProjection);

}

///=======================================================================
/// ダメ―ジ演出
///=======================================================================
void Player::DamageRendition() {
	//if (isDamage_) {
	//	damageTime_ -= Frame::DeltaTimeRate();

	//	// ダメージ時間がまだ残っている場合

	//	blinkTimer_ += Frame::DeltaTimeRate();

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



///=========================================================
/// 移動の入力処理
///==========================================================
Vector3 Player::GetInputDirecton() {

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
	if (input->GetJoystickState(0, joyState)) {
		velocity.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX;
		velocity.z += (float)joyState.Gamepad.sThumbLY / SHRT_MAX;
	}

	return velocity;
}

///=========================================================
/// 移動
///==========================================================
void Player::Move(const float& speed) {

	/// Inuputから速度代入
	direction_ = GetInputDirecton();

	/// 移動処理
	if (GetIsMoving()) {
		// 移動量に速さを反映
		direction_ = Vector3::Normalize(direction_) * (speed)*Frame::DeltaTimeRate();
		// 移動ベクトルをカメラの角度だけ回転する
		Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_->rotation_.y);
		direction_ = TransformNormal(direction_, rotateMatrix);
		// 移動
		transform_.translation_ += direction_;
		// 目標角度
		objectiveAngle_ = std::atan2(direction_.x, direction_.z);
		// 最短角度補間
		transform_.rotation_.y = LerpShortAngle(transform_.rotation_.y, objectiveAngle_, 0.3f);

	}
	else if (pLockOn_ && pLockOn_->GetEnemyTarget()) {
		Vector3 differectialVector = pLockOn_->GetTargetPosition() - GetWorldPosition();

		// Y軸周り角度(θy)
		transform_.rotation_.y = std::atan2(differectialVector.x, differectialVector.z);
	}
}

///=========================================================
/// 動いているか
///==========================================================
bool Player::GetIsMoving() {

	Input* input = Input::GetInstance();
	bool isMoving = false;
	const float thresholdValue = 0.3f;
	Vector3 StickVelocity;
	Vector3 keyBoradVelocity;

	/// ----------------------------------------------------------
	///  keyBorad
	/// ----------------------------------------------------------
	if (input->PushKey(DIK_W) || input->PushKey(DIK_A) || input->PushKey(DIK_S) || input->PushKey(DIK_D)) {

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
		if ((keyBoradVelocity).Length() > 0) {
			isMoving = true;
		}
	}
	else {
		/// ----------------------------------------------------------
		///  JoyStick
		/// ----------------------------------------------------------
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			// 移動量
			StickVelocity = { (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0, (float)joyState.Gamepad.sThumbLY / SHRT_MAX };
			if ((StickVelocity).Length() > thresholdValue) {
				isMoving = true;
			}
		}
	}
	return isMoving;
}

/// ===================================================
///  Player Jump
/// ===================================================
void Player::Jump(float& speed) {
	//ジャンプスピード加算
	transform_.translation_.y += speed;

	// 落下スピード限界まで重力かける
	speed = max(speed - (gravity_ * Frame::DeltaTimeRate()), fallSpeedLimit_);

	// 着地
	if (transform_.translation_.y <= Player::InitY_) {
		transform_.translation_.y = Player::InitY_;

		// ジャンプ終了
		ChangeBehavior(std::make_unique<PlayerRoot>(this));
	}
}

///=========================================================
///　移動制限
///==========================================================
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

	///--------------------------------------------------------------------------------
	///範囲外なら戻す
	///--------------------------------------------------------------------------------

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

///=========================================================
///　落ちる
///==========================================================
void Player::Fall() {
	if (dynamic_cast<PlayerJump*>(behavior_.get())) return;

	// 移動
	transform_.translation_.y += fallSpeed_;
	// 加速する
	fallSpeed_ = max(fallSpeed_ - (gravity_ * Frame::DeltaTimeRate()), -0.75f);

	// 着地
	if (transform_.translation_.y <= Player::InitY_) {
		transform_.translation_.y = Player::InitY_;
		fallSpeed_ = 0.0f;
	}
}



///=========================================================
/// パラメータ調整
///==========================================================
void Player::AdjustParm() {
	SetValues();
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Player")) {

		/// 位置
		ImGui::SeparatorText("Transform");
		ImGui::DragFloat3("Position", &transform_.translation_.x, 0.1f);

		///　Floatのパラメータ
		ImGui::SeparatorText("FloatParamater");
		ImGui::DragFloat("jumpSpeed", &jumpSpeed_, 0.01f);
		ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f);
		ImGui::DragFloat("Gravity", &gravity_, 0.01f);

		/// コンボパラメータ
		if (ImGui::CollapsingHeader("NormalCombo")) {
			ImGui::SeparatorText("FirstCombo");   /// 1コンボ目

			ImGui::DragFloat("PTime1",
				&normalComboParms_[0].permissionTime,
				0.01f);


			ImGui::DragFloat("rushDistance", &rushDistance_, 0.01f);
			ImGui::DragFloat("rushEaseMax", &rushEaseMax_, 0.01f, 0);

			ImGui::DragFloat("PunchEaseMax1",
				&normalComboParms_[0].punchEaseMax,
				0.01f);

			ImGui::DragFloat("PunchReach1",
				&normalComboParms_[0].punchReach,
				0.01f);

			ImGui::SeparatorText("SecondCombo");  /// 2コンボ目

			ImGui::DragFloat("PTime2",
				&normalComboParms_[1].permissionTime,
				0.01f);

			ImGui::DragFloat("PunchEaseMax2",
				&normalComboParms_[1].punchEaseMax,
				0.01f);

			ImGui::DragFloat("PunchReach2",
				&normalComboParms_[1].punchReach,
				0.01f);

			ImGui::SeparatorText("ThirdCombo");   /// 3コンボ目

			ImGui::DragFloat("PTime3",
				&normalComboParms_[2].permissionTime,
				0.01f);

			ImGui::DragFloat("PunchEaseMax3",
				&normalComboParms_[2].punchEaseMax,
				0.01f);

			ImGui::DragFloat("UpperPosY", &upperPosY_, 0.1f);

			ImGui::SeparatorText("ForthCombo");   /// 4コンボ目

			ImGui::DragFloat("PTime4",
				&normalComboParms_[3].permissionTime,
				0.01f);
		}

		if (ImGui::CollapsingHeader("JumpCombo")) {
			ImGui::SeparatorText("FirstCombo");   /// 1コンボ目

			ImGui::DragFloat("JPTime1",
				&jumpComboParms_[0].permissionTime,
				0.01f);

			ImGui::DragFloat("JPunchEaseMax1",
				&jumpComboParms_[0].punchEaseMax,
				0.01f);

			ImGui::SeparatorText("SecondCombo");   /// 2コンボ目

			ImGui::DragFloat("JPTime2",
				&jumpComboParms_[1].permissionTime,
				0.01f);

			ImGui::DragFloat("JPunchEaseMax2",
				&jumpComboParms_[1].punchEaseMax,
				0.01f);

			ImGui::DragFloat("JPunchReach2",
				&jumpComboParms_[1].punchReach,
				0.01f);

		}

		/// セーブとロード
		globalParameter_->ParmSaveForImGui(groupName_);
		ParmLoadForImGui();
	}

	/// 手のパラメータ調整
	leftHand_->AdjustParm();
	rightHand_->AdjustParm();

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

///=========================================================
/// ダメージ受ける
///==========================================================
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

void Player::SetLockOn(LockOn* lockon) {
	pLockOn_ = lockon;
}

///=========================================================
/// Collision
///==========================================================



///=========================================================
///振る舞い切り替え
///==========================================================
void Player::ChangeBehavior(std::unique_ptr<BasePlayerBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}
void Player::ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	comboBehavior_ = std::move(behavior);
}


///=================================================================================
/// ロード
///=================================================================================
void Player::ParmLoadForImGui() {

	// ロードボタン
	if (ImGui::Button(std::format("Load {}", groupName_).c_str())) {

		globalParameter_->LoadFile(groupName_);
		// セーブ完了メッセージ
		ImGui::Text("Load Successful: %s", groupName_.c_str());
		ApplyGlobalParameter();
	}
}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void Player::AddParmGroup() {

	globalParameter_->AddItem(groupName_, "Translate", transform_.translation_);
	globalParameter_->AddItem(groupName_, "JumpSpeed", jumpSpeed_);
	globalParameter_->AddItem(groupName_, "rushDistance", rushDistance_);
	globalParameter_->AddItem(groupName_, "rushEaseMax", rushEaseMax_);
	globalParameter_->AddItem(groupName_, "UpperPosY", upperPosY_);
	globalParameter_->AddItem(groupName_, "MoveSpeed", moveSpeed_);
	globalParameter_->AddItem(groupName_, "Gravity", gravity_);

	/// コンボ持続時間
	for (uint32_t i = 0; i < normalComboParms_.size(); ++i) {
		globalParameter_->AddItem(
			groupName_,
			"NComboPTime" + std::to_string(int(i + 1)),
			normalComboParms_[i].permissionTime);

		globalParameter_->AddItem(
			groupName_,
			"NComboPunchEaseTime" + std::to_string(int(i + 1)),
			normalComboParms_[i].punchEaseMax);

		globalParameter_->AddItem(
			groupName_,
			"NComboPunchReach" + std::to_string(int(i + 1)),
			normalComboParms_[i].punchReach);
	}

	/// コンボ持続時間(ジャンプ)
	for (uint32_t i = 0; i < jumpComboParms_.size(); ++i) {
		globalParameter_->AddItem(
			groupName_,
			"JComboPTime" + std::to_string(int(i + 1)),
			normalComboParms_[i].permissionTime);

		globalParameter_->AddItem(
			groupName_,
			"JComboPunchEaseTime" + std::to_string(int(i + 1)),
			normalComboParms_[i].punchEaseMax);

		globalParameter_->AddItem(
			groupName_,
			"JComboPunchReach" + std::to_string(int(i + 1)),
			normalComboParms_[i].punchReach);
	}
}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void Player::SetValues() {

	globalParameter_->SetValue(groupName_, "Translate", transform_.translation_);
	globalParameter_->SetValue(groupName_, "JumpSpeed", jumpSpeed_);
	globalParameter_->SetValue(groupName_, "rushDistance", rushDistance_);
	globalParameter_->SetValue(groupName_, "rushEaseMax", rushEaseMax_);
	globalParameter_->SetValue(groupName_, "UpperPosY", upperPosY_);
	globalParameter_->SetValue(groupName_, "MoveSpeed", moveSpeed_);
	globalParameter_->SetValue(groupName_, "Gravity", gravity_);

	/// コンボ持続時間
	for (uint32_t i = 0; i < normalComboParms_.size(); ++i) {
		globalParameter_->SetValue(
			groupName_,
			"NComboPTime" + std::to_string(int(i + 1)),
			normalComboParms_[i].permissionTime);

		globalParameter_->SetValue(
			groupName_,
			"NComboPunchEaseTime" + std::to_string(int(i + 1)),
			normalComboParms_[i].punchEaseMax);

		globalParameter_->SetValue(
			groupName_,
			"NComboPunchReach" + std::to_string(int(i + 1)),
			normalComboParms_[i].punchReach);
	}

	/// コンボ持続時間(ジャンプ)
	for (uint32_t i = 0; i < jumpComboParms_.size(); ++i) {
		globalParameter_->SetValue(
			groupName_,
			"JComboPTime" + std::to_string(int(i + 1)),
			jumpComboParms_[i].permissionTime);

		globalParameter_->SetValue(
			groupName_,
			"JComboPunchEaseTime" + std::to_string(int(i + 1)),
			jumpComboParms_[i].punchEaseMax);

		globalParameter_->SetValue(
			groupName_,
			"JComboPunchReach" + std::to_string(int(i + 1)),
			jumpComboParms_[i].punchReach);
	}
}


///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void Player::ApplyGlobalParameter() {
	// Position
	transform_.translation_ = globalParameter_->GetValue<Vector3>(groupName_, "Translate");
	jumpSpeed_ = globalParameter_->GetValue<float>(groupName_, "JumpSpeed");
	rushDistance_ = globalParameter_->GetValue<float>(groupName_, "rushDistance");
	rushEaseMax_ = globalParameter_->GetValue<float>(groupName_, "rushEaseMax");
	upperPosY_= globalParameter_->GetValue<float>(groupName_, "UpperPosY");
	moveSpeed_ = globalParameter_->GetValue<float>(groupName_, "MoveSpeed");
	gravity_ = globalParameter_->GetValue<float>(groupName_, "Gravity");

	/// コンボ持続時間
	for (uint32_t i = 0; i < normalComboParms_.size(); ++i) {
		normalComboParms_[i].permissionTime = globalParameter_->GetValue<float>(
			groupName_,
			"NComboPTime" + std::to_string(int(i + 1)));

		normalComboParms_[i].punchEaseMax = globalParameter_->GetValue<float>(
			groupName_,
			"NComboPunchEaseTime" + std::to_string(int(i + 1)));

		normalComboParms_[i].punchReach = globalParameter_->GetValue<float>(
			groupName_,
			"NComboPunchReach" + std::to_string(int(i + 1)));
	}

	/// コンボ持続時間
	for (uint32_t i = 0; i < jumpComboParms_.size(); ++i) {
		jumpComboParms_[i].permissionTime = globalParameter_->GetValue<float>(
			groupName_,
			"JComboPTime" + std::to_string(int(i + 1)));

		jumpComboParms_[i].punchEaseMax = globalParameter_->GetValue<float>(
			groupName_,
			"JComboPunchEaseTime" + std::to_string(int(i + 1)));

		jumpComboParms_[i].punchReach = globalParameter_->GetValue<float>(
			groupName_,
			"JComboPunchReach" + std::to_string(int(i + 1)));
	}
}

float Player::GetPunchEaseMax(NormalComboNum index)const {
	return normalComboParms_[index].punchEaseMax;
}

float Player::GetPunchReach(NormalComboNum index)const {
	return normalComboParms_[index].punchReach;
}

float Player::GetWaitTime(NormalComboNum index)const {
	return normalComboParms_[index].permissionTime;
}

float Player::GetJWaitTime(JumpComboNum index)const {
	return jumpComboParms_[index].permissionTime;
}
float Player::GetJPunchEaseMax(JumpComboNum index)const {
	return jumpComboParms_[index].punchEaseMax;
}

float Player::GetJPunchReach(JumpComboNum index)const {
	return jumpComboParms_[index].punchReach;
}