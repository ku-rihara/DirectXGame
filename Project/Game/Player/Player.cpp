#include "Player.h"
#include"audio/Audio.h"

/// input
#include"JoyState/JoyState.h"

/// frame
#include"Frame/Frame.h"
#include"Lighrt/Light.h"
#include"Lighrt/SpotLightManager.h"
/// math
#include"MathFunction.h"

/// object
#include"Field/Field.h"
#include"LockOn/LockOn.h"
#include"Enemy/BaseEnemy.h"
#include"CollisionBox/EnemyCollisionBox.h"
#include"base/TextureManager.h"
#include"utility/Particle/ParticleCommon.h"

/// behavior
#include"PlayerBehavior/PlayerRoot.h"
#include"PlayerBehavior/PlayerJump.h"
#include"ComboAttackBehavior/ComboAttackRoot.h"
#include"ComboAttackBehavior/ComboAttackJumpSecond.h"
#include"TitleBehavior/TitleFirstFall.h"

/// imgui
#include<imgui.h>

///=========================================================
///　static 変数初期化
///==========================================================
float Player::InitY_ = 0.0f;

Player::Player() {}

///=========================================================
///　初期化
///==========================================================
void Player::Init() {

	///* model生成
	BaseObject::Init();	// 基底クラスの初期化 
	BaseObject::CreateModel("Player", ".obj");/// モデルセット
	fallParticleName_ = "fallParticle";

	cirlceTexture_ = TextureManager::GetInstance()->LoadTexture("Resources/Texture/circle.png");

	///* グローバルパラメータ
	globalParameter_ = GlobalParameter::GetInstance();
	globalParameter_->CreateGroup(groupName_, false);
	AddParmGroup();
	ApplyGlobalParameter();

	/// ライティング
	obj3d_->material_.materialData_->enableLighting = 2;

	///* 武器生成
	leftHand_ = std::make_unique<PlayerHandLeft>();
	rightHand_ = std::make_unique<PlayerHandRight>();
	headObj_.reset(Object3d::CreateModel("Player", ".obj"));

	// トランスフォーム初期化
	headTransform_.Init();
	leftHand_->Init();
	rightHand_->Init();

	///* ペアレント
	headTransform_.SetParent(&transform_);
	leftHand_->SetParent(&transform_);
	rightHand_->SetParent(&transform_);

	/// レールペアレント
	rightHand_->SetRailParent(&transform_);
	leftHand_->SetRailParent(&transform_);

	// パーティクル初期化
	fallEmitter_.reset(ParticleEmitter::CreateParticle(fallParticleName_, "DebugSphere", ".obj", 300));
	fallEmitter_->SetBlendMode(BlendMode::None);

	//パラメータセット
	transform_.translation_ = playerParams_.startPos_;

	// 音
	punchSoundID_ = Audio::GetInstance()->LoadWave("Resources/punchAir.wav");
	strongPunch_ = Audio::GetInstance()->LoadWave("Resources/StrongPunch.wav");
	fallSound_ = Audio::GetInstance()->LoadWave("Resources/PlayerFall.wav");

	/// 通常モードから
	ChangeBehavior(std::make_unique<PlayerRoot>(this));
	ChangeComboBehavior(std::make_unique<ComboAttackRoot>(this));
	/*ChangeTitleBehavior(std::make_unique<TitleFirstFall>(this));*/
}

///=========================================================
///　更新
///==========================================================
void Player::Update() {
	prePos_ = GetWorldPosition();// 前フレームの座標

	// 落ちるパーティクルエミッター
	fallEmitter_->SetTargetPosition(GetWorldPosition());
	fallEmitter_->Update();

	/// 振る舞い処理(コンボ攻撃中は中止)
	if (dynamic_cast<ComboAttackRoot*>(comboBehavior_.get())) {
		behavior_->Update();
	}

	// ライト位置
	Light::GetInstance()->GetSpotLightManager()->GetSpotLight(0)->SetPosition(Vector3(
		transform_.translation_.x,
		transform_.translation_.y + 5.0f,
		transform_.translation_.z));


	comboBehavior_->Update();	  ///　コンボ攻撃攻撃
	MoveToLimit();                ///　移動制限
	FallEffectUpdate();

	UpdateMatrix();
}

// タイトル更新
void Player::TitleUpdate() {
	fallEmitter_->SetTargetPosition(GetWorldPosition());
	fallEmitter_->Update();
	FallEffectUpdate();

	// ライト位置
	Light::GetInstance()->GetSpotLightManager()->GetSpotLight(0)->SetPosition(Vector3(
		transform_.translation_.x,
		transform_.translation_.y + 5.0f,
		transform_.translation_.z));

	titleBehavior_->Update();
	/// 行列更新
	UpdateMatrix();
}

///=========================================================
///　描画
///==========================================================
void Player::Draw(const ViewProjection& viewProjection) {

	headObj_->Draw(headTransform_, viewProjection);
	leftHand_->Draw(viewProjection);
	rightHand_->Draw(viewProjection);
}

void Player::EffectDraw(const ViewProjection& viewProjection) {
	// 各エフェクトを更新
	effects_.reverse();
	for (std::unique_ptr<Effect>& effect : effects_) {
		if (effect) {
			effect->Draw(viewProjection);
		}
	}
	effects_.reverse();
}

void Player::SoundPunch() {
	Audio::GetInstance()->PlayWave(punchSoundID_, 0.5f);
}
void Player::SoundStrongPunch() {
	Audio::GetInstance()->PlayWave(strongPunch_, 0.5f);
}
void Player::FallSound() {
	Audio::GetInstance()->PlayWave(fallSound_, 0.2f);
}
///=======================================================================
/// ダメ―ジ演出
///=======================================================================
void Player::DamageRendition() {

}

void Player::FallParticleEmit() {
	fallEmitter_->Emit();
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
		if (dynamic_cast<ComboAttackRoot*>(comboBehavior_.get())) {
			transform_.rotation_.y = LerpShortAngle(transform_.rotation_.y, objectiveAngle_, 0.3f);
		}
		FaceToTarget();
	} else {
		FaceToTarget();// ターゲットを向くか
	}
}

void Player::FaceToTarget() {
	if (pLockOn_ && pLockOn_->GetEnemyTarget()) {
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

	/// ---------------------------------------------------------------------
	///  keyBorad
	/// ---------------------------------------------------------------------
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
	} else {
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


/// ===================================================
///  Player Jump
/// ===================================================
void Player::Jump(float& speed) {
	// 移動
	transform_.translation_.y += speed * Frame::DeltaTime();
	Fall(speed, true);

}

///=========================================================
///　落ちる
///==========================================================
void Player::Fall(float& speed, const bool& isJump) {

	if (!isJump) {
		// 移動
		transform_.translation_.y += speed * Frame::DeltaTime();
	}

	// 加速する
	speed = max(speed - (playerParams_.gravity * Frame::DeltaTime()), -playerParams_.fallSpeedLimit);

	// 着地
	if (transform_.translation_.y <= Player::InitY_) {
		transform_.translation_.y = Player::InitY_;
		speed = 0.0f;
		if (!dynamic_cast<PlayerJump*>(behavior_.get()))return;
		// ジャンプ終了
		ChangeBehavior(std::make_unique<PlayerRoot>(this));
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
		ImGui::DragFloat("jumpSpeed", &playerParams_.jumpSpeed, 0.01f);
		ImGui::DragFloat("MoveSpeed", &playerParams_.moveSpeed, 0.01f);
		ImGui::DragFloat("Gravity", &playerParams_.gravity, 0.01f);
		ImGui::DragFloat("FallSpeed", &playerParams_.fallSpeed, 0.01f);
		ImGui::DragFloat("fallSpeedLimit", &playerParams_.fallSpeedLimit, 0.1f);
		ImGui::DragFloat("rushDistance", &playerParams_.rushDistance, 0.01f);
		ImGui::DragFloat("rushEaseMax", &playerParams_.rushEaseMax, 0.01f, 0);
		ImGui::DragFloat("UpperPosY", &playerParams_.upperPosY, 0.1f);
		ImGui::SliderAngle("attackRotate", &playerParams_.attackRotate, 0, 720);
		ImGui::DragFloat("attackRotateEaseT", &playerParams_.attackRotateEaseT, 0.01f);
		ImGui::DragFloat("attackFloatEaseT", &playerParams_.attackFloatEaseT, 0.01f);
		ImGui::DragFloat("attackFloatValue", &playerParams_.attackFloatValue, 0.01f);

		/// コンボパラメータ
		if (ImGui::CollapsingHeader("NormalCombo")) {
			ImGui::SeparatorText("FirstCombo");   /// 1コンボ目

			ImGui::DragFloat("PTime1", &normalComboParms_[0].waitTime, 0.01f);
			ImGui::DragFloat("PunchEaseMax1", &normalComboParms_[0].attackEaseMax, 0.01f);
			ImGui::DragFloat("PunchReach1", &normalComboParms_[0].attackReach, 0.01f);

			ImGui::SeparatorText("SecondCombo");  /// 2コンボ目

			ImGui::DragFloat("PTime2", &normalComboParms_[1].waitTime, 0.01f);
			ImGui::DragFloat("PunchEaseMax2", &normalComboParms_[1].attackEaseMax, 0.01f);
			ImGui::DragFloat("PunchReach2", &normalComboParms_[1].attackReach, 0.01f);

			ImGui::SeparatorText("ThirdCombo");   /// 3コンボ目

			ImGui::DragFloat("PTime3", &normalComboParms_[2].waitTime, 0.01f);
			ImGui::DragFloat("PunchEaseMax3", &normalComboParms_[2].attackEaseMax, 0.01f);

			ImGui::SeparatorText("ForthCombo");   /// 4コンボ目

			ImGui::DragFloat("PTime4", &normalComboParms_[3].waitTime, 0.01f);
		}

		if (ImGui::CollapsingHeader("JumpCombo")) {
			ImGui::SeparatorText("FirstCombo");   /// 1コンボ目

			ImGui::DragFloat("JPTime1", &jumpComboParms_[0].waitTime, 0.01f);

			ImGui::DragFloat("JPunchEaseMax1", &jumpComboParms_[0].attackEaseMax, 0.01f);

			ImGui::SeparatorText("SecondCombo");   /// 2コンボ目

			ImGui::DragFloat("JPTime2", &jumpComboParms_[1].waitTime, 0.01f);
			ImGui::DragFloat("JPunchEaseMax2", &jumpComboParms_[1].attackEaseMax, 0.01f);
			ImGui::DragFloat("JPunchReach2", &jumpComboParms_[1].attackReach, 0.01f);
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

void Player::FallEffectInit(const Vector3& pos) {
	std::unique_ptr<Effect> effect = std::make_unique<Effect>();

	effect->Init(pos);
	effects_.push_back(std::move(effect));
}


void Player::FallEffectUpdate() {
	// 各エフェクトを更新
	for (std::unique_ptr<Effect>& effect : effects_) {
		if (effect) {
			effect->Update();
		}
	}

	// 完了したエフェクトを消す
	effects_.erase(std::remove_if(effects_.begin(), effects_.end(), [](const std::unique_ptr<Effect>& effect) { return effect->IsFinished(); }), effects_.end());
}

void Player::SetGameCamera(GameCamera* gamecamera) {
	pGameCamera_ = gamecamera;
}

///==============================================================================
/// ダメージ受ける
///===============================================================================
void Player::TakeDamage() {
	/*if (!isDamage_) {
		life_--;
		isDamage_ = true;
		damageTime_ = damageCollTime_;
	}*/
}

///==============================================================================
/// Class Set
///===============================================================================

void Player::SetLockOn(LockOn* lockon) {
	pLockOn_ = lockon;
}

///==============================================================================
/// Collision
///===============================================================================



///==============================================================================
///振る舞い切り替え
///===============================================================================
void Player::ChangeBehavior(std::unique_ptr<BasePlayerBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
}
void Player::ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	comboBehavior_ = std::move(behavior);
}

void Player::ChangeTitleBehavior(std::unique_ptr<BaseTitleBehavior>behavior) {
	titleBehavior_ = std::move(behavior);
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

	globalParameter_->AddItem(groupName_, "Translate", playerParams_.startPos_);
	globalParameter_->AddItem(groupName_, "JumpSpeed", playerParams_.jumpSpeed);
	globalParameter_->AddItem(groupName_, "rushDistance", playerParams_.rushDistance);
	globalParameter_->AddItem(groupName_, "rushEaseMax", playerParams_.rushEaseMax);
	globalParameter_->AddItem(groupName_, "UpperPosY", playerParams_.upperPosY);
	globalParameter_->AddItem(groupName_, "MoveSpeed", playerParams_.moveSpeed);
	globalParameter_->AddItem(groupName_, "Gravity", playerParams_.gravity);
	globalParameter_->AddItem(groupName_, "FallSpeed", playerParams_.fallSpeed);
	globalParameter_->AddItem(groupName_, "FallSpeedLimit", playerParams_.fallSpeedLimit);
	globalParameter_->AddItem(groupName_, "attackRotate", playerParams_.attackRotate);
	globalParameter_->AddItem(groupName_, "attackRotateEaseT", playerParams_.attackRotateEaseT);
	globalParameter_->AddItem(groupName_, "attackFloatEaseT_", playerParams_.attackFloatEaseT);
	globalParameter_->AddItem(groupName_, "attackFloatValue_", playerParams_.attackFloatValue);

	/// コンボ持続時間
	for (uint32_t i = 0; i < normalComboParms_.size(); ++i) {
		globalParameter_->AddItem(groupName_, "NComboPTime" + std::to_string(int(i + 1)), normalComboParms_[i].waitTime);
		globalParameter_->AddItem(groupName_, "NComboPunchEaseTime" + std::to_string(int(i + 1)), normalComboParms_[i].attackEaseMax);
		globalParameter_->AddItem(groupName_, "NComboPunchReach" + std::to_string(int(i + 1)), normalComboParms_[i].attackReach);
	}

	/// コンボ持続時間(ジャンプ)
	for (uint32_t i = 0; i < jumpComboParms_.size(); ++i) {
		globalParameter_->AddItem(groupName_, "JComboPTime" + std::to_string(int(i + 1)), normalComboParms_[i].waitTime);
		globalParameter_->AddItem(groupName_, "JComboPunchEaseTime" + std::to_string(int(i + 1)), normalComboParms_[i].attackEaseMax);
		globalParameter_->AddItem(groupName_, "JComboPunchReach" + std::to_string(int(i + 1)), normalComboParms_[i].attackReach);
	}
}


///=================================================================================
///パラメータをグループに追加
///=================================================================================
void Player::SetValues() {

	globalParameter_->SetValue(groupName_, "Translate", playerParams_.startPos_);
	globalParameter_->SetValue(groupName_, "JumpSpeed", playerParams_.jumpSpeed);
	globalParameter_->SetValue(groupName_, "rushDistance", playerParams_.rushDistance);
	globalParameter_->SetValue(groupName_, "rushEaseMax", playerParams_.rushEaseMax);
	globalParameter_->SetValue(groupName_, "UpperPosY", playerParams_.upperPosY);
	globalParameter_->SetValue(groupName_, "MoveSpeed", playerParams_.moveSpeed);
	globalParameter_->SetValue(groupName_, "Gravity", playerParams_.gravity);
	globalParameter_->SetValue(groupName_, "FallSpeed", playerParams_.fallSpeed);
	globalParameter_->SetValue(groupName_, "FallSpeedLimit", playerParams_.fallSpeedLimit);
	globalParameter_->SetValue(groupName_, "attackRotate", playerParams_.attackRotate);
	globalParameter_->SetValue(groupName_, "attackRotateEaseT", playerParams_.attackRotateEaseT);
	globalParameter_->SetValue(groupName_, "attackFloatEaseT_", playerParams_.attackFloatEaseT);
	globalParameter_->SetValue(groupName_, "attackFloatValue_", playerParams_.attackFloatValue);

	/// コンボ持続時間
	for (uint32_t i = 0; i < normalComboParms_.size(); ++i) {
		globalParameter_->SetValue(groupName_, "NComboPTime" + std::to_string(int(i + 1)), normalComboParms_[i].waitTime);
		globalParameter_->SetValue(groupName_, "NComboPunchEaseTime" + std::to_string(int(i + 1)), normalComboParms_[i].attackEaseMax);
		globalParameter_->SetValue(groupName_, "NComboPunchReach" + std::to_string(int(i + 1)), normalComboParms_[i].attackReach);
	}

	/// コンボ持続時間(ジャンプ)
	for (uint32_t i = 0; i < jumpComboParms_.size(); ++i) {
		globalParameter_->SetValue(groupName_, "JComboPTime" + std::to_string(int(i + 1)), jumpComboParms_[i].waitTime);
		globalParameter_->SetValue(groupName_, "JComboPunchEaseTime" + std::to_string(int(i + 1)), jumpComboParms_[i].attackEaseMax);
		globalParameter_->SetValue(groupName_, "JComboPunchReach" + std::to_string(int(i + 1)), jumpComboParms_[i].attackReach);
	}
}


///=====================================================
///  ImGuiからパラメータを得る
///===================================================== 
void Player::ApplyGlobalParameter() {

	playerParams_.startPos_ = globalParameter_->GetValue<Vector3>(groupName_, "Translate");
	playerParams_.jumpSpeed = globalParameter_->GetValue<float>(groupName_, "JumpSpeed");
	playerParams_.rushDistance = globalParameter_->GetValue<float>(groupName_, "rushDistance");
	playerParams_.rushEaseMax = globalParameter_->GetValue<float>(groupName_, "rushEaseMax");
	playerParams_.upperPosY = globalParameter_->GetValue<float>(groupName_, "UpperPosY");
	playerParams_.moveSpeed = globalParameter_->GetValue<float>(groupName_, "MoveSpeed");
	playerParams_.gravity = globalParameter_->GetValue<float>(groupName_, "Gravity");
	playerParams_.fallSpeed = globalParameter_->GetValue<float>(groupName_, "FallSpeed");
	playerParams_.fallSpeedLimit = globalParameter_->GetValue<float>(groupName_, "FallSpeedLimit");
	playerParams_.attackRotateEaseT = globalParameter_->GetValue<float>(groupName_, "attackRotateEaseT");
	playerParams_.attackRotate = globalParameter_->GetValue<float>(groupName_, "attackRotate");
	playerParams_.attackFloatEaseT = globalParameter_->GetValue<float>(groupName_, "attackFloatEaseT_");
	playerParams_.attackFloatValue = globalParameter_->GetValue<float>(groupName_, "attackFloatValue_");

	/// コンボ持続時間
	for (uint32_t i = 0; i < normalComboParms_.size(); ++i) {
		normalComboParms_[i].waitTime = globalParameter_->GetValue<float>(groupName_, "NComboPTime" + std::to_string(int(i + 1)));
		normalComboParms_[i].attackEaseMax = globalParameter_->GetValue<float>(groupName_, "NComboPunchEaseTime" + std::to_string(int(i + 1)));
		normalComboParms_[i].attackReach = globalParameter_->GetValue<float>(groupName_, "NComboPunchReach" + std::to_string(int(i + 1)));
	}

	/// コンボ持続時間
	for (uint32_t i = 0; i < jumpComboParms_.size(); ++i) {
		jumpComboParms_[i].waitTime = globalParameter_->GetValue<float>(groupName_, "JComboPTime" + std::to_string(int(i + 1)));
		jumpComboParms_[i].attackEaseMax = globalParameter_->GetValue<float>(groupName_, "JComboPunchEaseTime" + std::to_string(int(i + 1)));
		jumpComboParms_[i].attackReach = globalParameter_->GetValue<float>(groupName_, "JComboPunchReach" + std::to_string(int(i + 1)));
	}
}

/// =========================================================================================
/// getter method
/// =========================================================================================


void Player::SetTitleBehavior() {
	ChangeTitleBehavior(std::make_unique<TitleFirstFall>(this));
}

void Player::UpdateMatrix() {
	/// 行列更新
	headTransform_.UpdateMatrix();
	leftHand_->Update();
	rightHand_->Update();
	BaseObject::Update();
}
void Player::OnCollisionStay([[maybe_unused]] BaseCollider* other) {

	if (dynamic_cast<ComboAttackJumpSecond*>(comboBehavior_.get()))return;

	if (EnemyCollisionBox* enemy = dynamic_cast<EnemyCollisionBox*>(other)) {
		// 敵の中心座標を取得
		const Vector3& enemyPosition = enemy->GetCollisionPos();

		// プレイヤーと敵の位置の差分ベクトルを計算
		Vector3 delta = transform_.translation_ - enemyPosition;

		// スケール取得
		Vector3 enemyScale = enemy->GetCollisonScale();
		Vector3 myScale = GetCollisonScale();

		// 押し出す距離の計算
		float pushDistanceX = (enemyScale.x + myScale.x) / 2.0f + 0.1f;
		float pushDistanceZ = (enemyScale.z + myScale.z) / 2.0f + 0.1f;

		// 実際の押し戻し距離を計算
		float pushAmountX = pushDistanceX - std::abs(delta.x);
		float pushAmountZ = pushDistanceZ - std::abs(delta.z);

		// ワープを防ぐために0以下の値を無効化
		pushAmountX = max(0.0f, pushAmountX);
		pushAmountZ = max(0.0f, pushAmountZ);

		// 押し戻し方向
		Vector3 pushDirection = { 0, 0, 0 };
		float pushDistance = 0.0f;

		if (pushAmountX > 0.0f && pushAmountZ > 0.0f) {
			// XとZ両方めり込んでいる場合
			if (pushAmountX > pushAmountZ) {
				pushDistance = pushAmountX;
				pushDirection = { delta.x > 0 ? 1.0f : -1.0f, 0, 0 };
			} else {
				pushDistance = pushAmountZ;
				pushDirection = { 0, 0, delta.z > 0 ? 1.0f : -1.0f };
			}
			/// それぞれ片方ずるめり込んでいる
		} else if (pushAmountX > 0.0f) {
			pushDistance = pushAmountX;
			pushDirection = { delta.x > 0 ? 1.0f : -1.0f, 0, 0 };
		} else if (pushAmountZ > 0.0f) {
			pushDistance = pushAmountZ;
			pushDirection = { 0, 0, delta.z > 0 ? 1.0f : -1.0f };
		}

		// ワープを防ぐため、最大移動量を制限
		 float MAX_PUSH_DISTANCE = 0.5f;
		pushDistance = std::min(pushDistance, MAX_PUSH_DISTANCE);

		// 実際に押し戻す
		if (pushDistance > 0.0f) {
			transform_.translation_ += pushDirection * pushDistance;
		}
	}
}


Vector3 Player::GetCollisionPos() const {
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 1.5f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);
	return worldPos;
}