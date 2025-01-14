#include "BaseEnemy.h"
// bvehaivor
#include"Behavior/EnemyChasePlayer.h"
#include"Behavior/EnemyHitBackDamage.h"
#include"Behavior/EnemyUpperDamage.h"
#include"Behavior/EnemyStopDamage.h"
#include"Behavior/EnemyThrustDamage.h"
#include"Behavior/EnemyBoundDamage.h"

/// collisionBox
#include"CollisionBox/PunchCollisionBox.h"
#include"CollisionBox/UpperCollisionBox.h"
#include"CollisionBox/StopCollisionBox.h"
#include"CollisionBox/ThrustCollisionBox.h"
#include"CollisionBox/FallCollisionBox.h"
#include"CollisionBox/RushCollisionBox.h"

#include"Matrix4x4.h"
#include"Player/Player.h"
#include"base/TextureManager.h"
#include"Frame/Frame.h"
#include"utility/Particle/ParticleCommon.h"

///=========================================================
///　static 変数初期化
///==========================================================
float BaseEnemy::InitY_ = 0.5f;
Vector3 BaseEnemy::InitScale_ = Vector3::UnitVector()*2;

BaseEnemy::BaseEnemy() {

}

///========================================================
///  初期化
///========================================================
void BaseEnemy::Init(const Vector3& spownPos) {
	// 基底クラスの初期化
	BaseObject::Init();
	

	HPMax_ = 105.0f;
	hp_ = HPMax_;
	hpbarSize_ = { HPMax_ ,90};
	
	hpbar_ = std::make_unique<EnemyHPBar>();
	hpbar_->Init(hpbarSize_);
	transform_.translation_ = spownPos;

	///spawn
	spawnEasing_.time = 0.0f;
	spawnEasing_.maxTime = 0.8f;
	transform_.scale_ = Vector3::ZeroVector();

	/// particleT
	thrustName_ = "TrushParticle";
	thrustEmit_.reset(ParticleEmitter::CreateParticle(thrustName_, "Stick",".obj", 200, false));
	thrustEmit_->SetBlendMode(BlendMode::None);
	thrustEmit_->SetIsBillBord(false);
	thrustEmit_->SetIsRotateForDirection(true);

	/// particleD
	damageName_ = "DamageParticle";
	damageEmitter_.reset(ParticleEmitter::CreateParticle(damageName_, "Plane", ".obj", 300, false));
	uint32_t handle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/circle.png");
	damageEmitter_->SetTextureHandle(handle);

	ChangeBehavior(std::make_unique<EnemyChasePlayer>(this));/// 追っかけ
}

///========================================================
/// 更新
///========================================================
void BaseEnemy::Update() {

	/// 生成処理
	spawnEasing_.time += Frame::DeltaTime();
	spawnEasing_.time = std::min(spawnEasing_.time, spawnEasing_.maxTime);
	transform_.scale_ = 
		EaseOutBack(Vector3::ZeroVector(), BaseEnemy::InitScale_,
		spawnEasing_.time, spawnEasing_.maxTime);

	behavior_->Update();

	damageEmitter_->SetTargetPosition(GetWorldPosition());
	damageEmitter_->Update();
	
	thrustEmit_->SetTargetPosition(GetWorldPosition());
	thrustEmit_->Update();

	FallEffectUpdate();

	
	// 体力がなくなったら死亡
	if (hp_ <= 0) {
		isdeath_=true;
		/*Audio::GetInstance()->PlayWave(deathSound_);*/
	}
	
	BaseObject::Update();
}
///========================================================
/// HpBar表示
///========================================================
void BaseEnemy::DisplayHpBar(const ViewProjection& viewProjection) {
	// ワールド座標からスクリーン座標に変換
	Vector3 positionScreen = ScreenTransform(GetWorldPosition(), viewProjection);
	// Vector2に格納
	Vector2 positionScreenV2(positionScreen.x-70, positionScreen.y - 90.0f);
	// Hpバーの座標確定
	Vector2 hpBarPosition = positionScreenV2;
	// Hpバーのサイズ
	hpbar_->SetSize(hpbarSize_);
	// HPBarスプライト
	hpbar_->SetPosition(hpBarPosition);
	// Hpバー更新
	hpbar_->Update(int(hp_));
}

Vector3 BaseEnemy::GetDirectionToTarget(const Vector3& target) {
	
	// 現在のボス位置を取得
	Vector3 enemyPosition =GetWorldPosition();

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
	for (std::unique_ptr<Effect>& effect : effects_) {
		if (effect) {
			effect->Draw(viewProjection);
		}
	}
	effects_.reverse();
	// 敵描画
	BaseObject::Draw(viewProjection);
}

///========================================================
/// Sprite描画
///========================================================
void BaseEnemy::SpriteDraw(const ViewProjection& viewProjection) {
	
		if (IsInView(viewProjection)) {
			hpbar_->Draw();
		}
}

void BaseEnemy::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	
}

void BaseEnemy::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	//普通のパンチに攻撃されたら
	if (dynamic_cast<PunchCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyHitBackDamage*>(behavior_.get())) {
		
			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyHitBackDamage>(this));
		}
		return;
	}

	//アッパーを食らったら
	if (dynamic_cast<UpperCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyUpperDamage*>(behavior_.get())) {
		
			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyUpperDamage>(this));
		}
		return;
	}

	//止まる攻撃
	if (dynamic_cast<StopCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyStopDamage*>(behavior_.get())) {
		
			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyStopDamage>(this));
		}
		return;
	}

	//月飛ばし攻撃
	if (dynamic_cast<ThrustCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyThrustDamage*>(behavior_.get())) {

			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyThrustDamage>(this));
		}
		return;
	}

	//落下攻撃
	if (dynamic_cast<FallCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyUpperDamage*>(behavior_.get())) {
		
			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyUpperDamage>(this));
		}

		return;
	}

	//突進攻撃
	if (dynamic_cast<RushCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyBoundDamage*>(behavior_.get())) {
		
			DamageForPar(damageParm_);
			ChangeBehavior(std::make_unique<EnemyBoundDamage>(this));	
		}

		return;
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


void BaseEnemy::ChangeBehavior(std::unique_ptr<BaseEnemyBehaivor>behavior) {
	//引数で受け取った状態を次の状態としてセット
	behavior_ = std::move(behavior);
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
	float decrementSize = HPMax_ * par;
	// HP減少
	hp_ -= decrementSize;

	////HPが0以下にならないように
	//if (hp_ <= 0) {

	//	hp_ = 0.0f;
	//	isdeath_ = true;
	//	//// 死亡処理
	//	//DeathMethod();
	//	//HP_ = 0.0f;
	//}
}

void BaseEnemy::DamageEmit() {
	damageEmitter_->Emit();
}

void BaseEnemy::ThrustEmit() {
	thrustEmit_->Emit();
}

void BaseEnemy::FallEffectInit(const Vector3& pos) {
	std::unique_ptr<Effect> effect = std::make_unique<Effect>();

	effect->Init(pos);

	effects_.push_back(std::move(effect));
}


void BaseEnemy::FallEffectUpdate() {
	// 各エフェクトを更新
	for (std::unique_ptr<Effect>& effect : effects_) {
		if (effect) {
			effect->Update();
		}
	}
	// 完了したエフェクトを消す
	effects_.erase(std::remove_if(effects_.begin(), effects_.end(), [](const std::unique_ptr<Effect>& effect) { return effect->IsFinished(); }), effects_.end());
}


void BaseEnemy::SetGameCamera(GameCamera* gamecamera) {
	pGameCamera_ = gamecamera;
}