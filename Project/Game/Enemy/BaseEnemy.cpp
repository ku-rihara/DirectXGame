#include "BaseEnemy.h"
// bvehaivor
#include"Behavior/EnemySpawn.h"
#include"Behavior/EnemyHitBackDamage.h"
#include"Behavior/EnemyUpperDamage.h"
#include"Behavior/EnemyStopDamage.h"
#include"Behavior/EnemyThrustDamage.h"
#include"Behavior/EnemyBoundDamage.h"
#include"Behavior/EnemyDamageRoot.h"
#include"Behavior/EnemyDeath.h"
#include"Enemy/EnemyManager.h"

/// collisionBox
#include"CollisionBox/AttackCollisionBox.h"

#include"Matrix4x4.h"
#include"Player/Player.h"
#include"base/TextureManager.h"
#include"Frame/Frame.h"
#include"utility/ParticleEditor/ParticleCommon.h"
#include"audio/Audio.h"

///=========================================================
///　static 変数初期化
///==========================================================

//Vector3 BaseEnemy::InitScale_ = Vector3::UnitVector() * 1.4f;

BaseEnemy::BaseEnemy() {

}

///========================================================
///  初期化
///========================================================
void BaseEnemy::Init(const Vector3& spownPos) {
	// 基底クラスの初期化
	BaseObject::Init();

	//HP
	HPMax_ = 105.0f;
	hp_ = HPMax_;
	hpbarSize_ = { HPMax_ ,90 };
	hpbar_ = std::make_unique<EnemyHPBar>();
	hpbar_->Init(hpbarSize_);

	/// transform
	transform_.translation_ = spownPos;
	transform_.translation_.y = paramater_.basePosY;
	transform_.scale_ = Vector3::ZeroVector();


	///collision
	collisionBox_ = std::make_unique<EnemyCollisionBox>();
	collisionBox_->Init();
	collisionBox_->SetSize(Vector3(3.2f, 3.2f, 3.2f));
	collisionBox_->IsAdapt(true);

	findSprite_ = std::make_unique<FindSprite>();
	notFindSprite_ = std::make_unique<NotFindSprite>();
	
	findSprite_->Init();
	notFindSprite_->Init();

	//audio
	deathSound_  = Audio::GetInstance()->LoadWave("./Resources/EnemyDeath.wav");
	thurstSound_ = Audio::GetInstance()->LoadWave("./Resources/Enemythurst.wav");

	ChangeBehavior(std::make_unique<EnemyDamageRoot>(this)); /// 振る舞い初期化
	ChangeMoveBehavior(std::make_unique<EnemySpawn>(this));  /// 振る舞い初期化
}

///========================================================
/// 更新
///========================================================
void BaseEnemy::Update() {

	if (dynamic_cast<EnemyDamageRoot*>(damageBehavior_.get())) {
		moveBehavior_->Update();
	}
	damageBehavior_->Update();

	FallEffectUpdate();

	BehaviorChangeDeath();

	collisionBox_->SetPosition(GetWorldPosition());
	collisionBox_->Update();
	BaseObject::Update();
}
///========================================================
/// HpBar表示
///========================================================
void BaseEnemy::DisplaySprite(const ViewProjection& viewProjection) {
	// ワールド座標からスクリーン座標に変換
	Vector3 positionScreen = ScreenTransform(GetWorldPosition(), viewProjection);
	// Vector2に格納
	Vector2 positionScreenV2(positionScreen.x - 70, positionScreen.y - 90.0f);
	// Hpバーの座標確定
	Vector2 hpBarPosition = positionScreenV2;
	// Hpバーのサイズ
	hpbar_->SetSize(hpbarSize_);
	// HPBarスプライト
	hpbar_->SetPosition(hpBarPosition);
	// Hpバー更新
	hpbar_->Update(int(hp_));

	Vector2 findPos(positionScreen.x, positionScreen.y - 100.0f);

	// HPBarスプライト
	findSprite_->SetPosition(findPos);
	// Hpバー更新
	findSprite_->Update();

	// HPBarスプライト
	notFindSprite_->SetPosition(findPos);
	// Hpバー更新
	notFindSprite_->Update();
}

Vector3 BaseEnemy::GetDirectionToTarget(const Vector3& target) {

	// 現在のボス位置を取得
	Vector3 enemyPosition = GetWorldPosition();

	// ターゲットへのベクトル
	Vector3 direction = target - enemyPosition;

	return direction;
}

///========================================================
/// 描画
///========================================================
void BaseEnemy::Draw(const ViewProjection& viewProjection) {

	// 各エフェクトを更新
	effects_.reverse();
    for (std::unique_ptr<ImpactEffect>& effect : effects_) {
		if (effect) {
			effect->Draw(viewProjection);
		}
	}
	effects_.reverse();

}

///========================================================
/// Sprite描画
///========================================================
void BaseEnemy::SpriteDraw(const ViewProjection& viewProjection) {

	if (IsInView(viewProjection)) {
		findSprite_->Draw();
		notFindSprite_->Draw();
		hpbar_->Draw();
	}
}

void BaseEnemy::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	//普通のパンチに攻撃されたら
	if (AttackCollisionBox* attackCollisionBox = dynamic_cast<AttackCollisionBox*>(other)) {

		switch (attackCollisionBox->attackType_)
		{
			///------------------------------------------------------------------
			/// 通常
			///------------------------------------------------------------------
		case AttackCollisionBox::AttackType::NORMAL:

			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyHitBackDamage>(this));

		}
	}
}

void BaseEnemy::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	//普通のパンチに攻撃されたら
	if (AttackCollisionBox* attackCollisionBox = dynamic_cast<AttackCollisionBox*>(other)) {

		switch (attackCollisionBox->attackType_)
		{
			
			///------------------------------------------------------------------
			/// アッパー
			///------------------------------------------------------------------
		case AttackCollisionBox::AttackType::UPPER:

			if (dynamic_cast<EnemyUpperDamage*>(damageBehavior_.get())) break;

			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyUpperDamage>(this));


			break;
			///------------------------------------------------------------------
			/// ストッパー
			///------------------------------------------------------------------
		case AttackCollisionBox::AttackType::STOPPER:
			if (dynamic_cast<EnemyStopDamage*>(damageBehavior_.get())) break;

			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyStopDamage>(this));

			break;
			///------------------------------------------------------------------
			/// 突き飛ばし	
			///------------------------------------------------------------------
		case AttackCollisionBox::AttackType::THRUST:
			if (dynamic_cast<EnemyThrustDamage*>(damageBehavior_.get())) break;

			DamageForPar(damageParm_*2.0f);
			ChangeBehavior(std::make_unique<EnemyThrustDamage>(this));


			break;
			///------------------------------------------------------------------
			/// 落下攻撃
			///------------------------------------------------------------------
		case AttackCollisionBox::AttackType::FALL:
			if (dynamic_cast<EnemyUpperDamage*>(damageBehavior_.get())) break;

			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyUpperDamage>(this));


			return;

			break;
			///------------------------------------------------------------------
			/// ストッパー
			///------------------------------------------------------------------
		case AttackCollisionBox::AttackType::RUSH:
			if (dynamic_cast<EnemyBoundDamage*>(damageBehavior_.get())) break;

			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyUpperDamage>(this));

			break;
		default:
			break;
		}
	}
}

Vector3 BaseEnemy::GetCollisionPos() const {
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);
	return worldPos;
}


void BaseEnemy::SetPlayer(Player* player) {
	pPlayer_ = player;
}


void BaseEnemy::ChangeBehavior(std::unique_ptr<BaseEnemyBehavior>behavior) {
	//引数で受け取った状態を次の状態としてセット
	damageBehavior_ = std::move(behavior);
}

void BaseEnemy::ChangeMoveBehavior(std::unique_ptr<BaseEnemyMoveBehavior>behavior) {
	moveBehavior_ = std::move(behavior);
}

void BaseEnemy::BehaviorChangeDeath() {
    if (hp_ > 0) {
        return;
    }
    if (dynamic_cast<EnemyDeath*>(damageBehavior_.get()) || dynamic_cast<EnemyThrustDamage*>(damageBehavior_.get())) {
        return;
	};

	isCollision_ = false;
	collisionBox_->SetIsCollision(false);
	ChangeBehavior(std::make_unique<EnemyDeath>(this));
}

// 視界にいるか
bool BaseEnemy::IsInView(const ViewProjection& viewProjection) const {
	Vector3 positionView = {};
	// 敵のロックオン座標を取得
	Vector3 positionWorld = GetWorldPosition();
	// ワールド→ビュー座標系
	positionView = MatrixTransform(positionWorld, viewProjection.matView_);
	// 距離条件チェック
	if (0.0f <= positionView.z && positionView.z <= 1280.0f) {
		// カメラ前方との角度を計算
		float actTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);

		// 角度条件チェック（コーンに収まっているか）
		return (std::fabsf(actTangent) <= std::fabsf(180 * 3.14f));
	}
	return false;
}

//割合によるダメージ
void BaseEnemy::DamageForPar(const float& par) {

	//割合によるインクる面とする値を決める
	float decrementSize = HPMax_ * (par / 100.0f);
	// HP減少
	hp_ -= decrementSize;
}

void BaseEnemy::DamageRenditionInit() {

	pEnemyManager_->DamageEffectShot(GetWorldPosition());
}

void BaseEnemy::ThrustRenditionInit() {
	//ガレキパーティクル
	pEnemyManager_->ThrustEmit(GetWorldPosition());
	
	Audio::GetInstance()->PlayWave(thurstSound_, 0.2f);
}

void BaseEnemy::DeathRenditionInit() {
	pEnemyManager_->DeathEmit(GetWorldPosition());
	Audio::GetInstance()->PlayWave(deathSound_, 0.5f);
}

void BaseEnemy::FallEffectInit(const Vector3& pos) {
    std::unique_ptr<ImpactEffect> effect = std::make_unique<ImpactEffect>();

	effect->Init(pos);
	effects_.push_back(std::move(effect));
}


void BaseEnemy::FallEffectUpdate() {
	// 各エフェクトを更新
    for (std::unique_ptr<ImpactEffect>& effect : effects_) {
		if (effect) {
			effect->Update();
		}
	}
	// 完了したエフェクトを消す
    effects_.erase(std::remove_if(effects_.begin(), effects_.end(), [](const std::unique_ptr<ImpactEffect>& effect) { return effect->IsFinished(); }), effects_.end());
}


/// ===================================================
///  BaseEnemy Jump
/// ===================================================
void BaseEnemy::Jump(float& speed, const float& fallSpeedLimit, const float& gravity) {
	// 移動
	transform_.translation_.y += speed * Frame::DeltaTimeRate();
	Fall(speed, fallSpeedLimit,gravity, true);

}

///=========================================================
///　落ちる
///==========================================================
void BaseEnemy::Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump) {

	if (!isJump) {
		// 移動
		transform_.translation_.y += speed * Frame::DeltaTimeRate();
	}

	// 加速する
	speed = max(speed - (gravity * Frame::DeltaTimeRate()), -fallSpeedLimit);

	// 着地
	if (transform_.translation_.y <= paramater_.basePosY) {
        transform_.translation_.y = paramater_.basePosY;
		speed = 0.0f;
		
	}
}

void BaseEnemy::SetGameCamera(GameCamera* gamecamera) {
	pGameCamera_ = gamecamera;
}

void  BaseEnemy::SetManager(EnemyManager* manager) {
	pEnemyManager_ = manager;
}

void BaseEnemy::BackToDamageRoot() {
	ChangeBehavior(std::make_unique<EnemyDamageRoot>(this));/// 追っかけ
}

void BaseEnemy::SetParamater(const Type& type, const Paramater& paramater) {
	type_ = type;
	paramater_ = paramater;
}
void  BaseEnemy::SetBodyColor(const Vector4& color){
	bodyObj_->objColor_.SetColor(color);
}

void BaseEnemy::RotateInit() {
	bodyTransform_.rotation_ = { 0.0f,0.0f,0.0f };
}

void BaseEnemy::ScaleReset() {
    transform_.scale_ = paramater_.initScale_;
}
