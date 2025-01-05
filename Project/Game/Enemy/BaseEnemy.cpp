#include "BaseEnemy.h"
// bvehaivor
#include"Behavior/EnemyChasePlayer.h"
#include"Behavior/EnemyHitBackDamage.h"
#include"Behavior/EnemyUpperDamage.h"
#include"Behavior/EnemyStopDamage.h"
#include"Behavior/EnemyThrustDamage.h"

/// collisionBox
#include"CollisionBox/PunchCollisionBox.h"
#include"CollisionBox/UpperCollisionBox.h"
#include"CollisionBox/StopCollisionBox.h"
#include"CollisionBox/ThrustCollisionBox.h"

#include"Matrix4x4.h"
#include"Player/Player.h"
#include"base/TextureManager.h"


///=========================================================
///　static 変数初期化
///==========================================================
float BaseEnemy::InitY_ = 0.5f;

BaseEnemy::BaseEnemy() {

}

///========================================================
///  初期化
///========================================================
void BaseEnemy::Init(const Vector3& spownPos) {
	// 基底クラスの初期化
	BaseObject::Init();
	/// モデルセット
	BaseObject::CreateModel("axis",".obj");

	transform_.translation_=spownPos;

	ChangeBehavior(std::make_unique<EnemyChasePlayer>(this));/// 追っかけ


}

///========================================================
/// 更新
///========================================================
void BaseEnemy::Update() {

	behavior_->Update();
	
	//// 体力がなくなったら死亡
	//if (hp_ <= 0 && !isBurst_) {
	//	behaviorRequest_ = Behavior::kDeath;
	//	/*Audio::GetInstance()->PlayWave(deathSound_);*/
	//}
	
	BaseObject::Update();
}
///========================================================
/// HpBar表示
///========================================================
void BaseEnemy::DisplayHpBar(const ViewProjection& viewProjection) {
	// ワールド座標からスクリーン座標に変換
	Vector3 positionScreen = ScreenTransform(GetWorldPosition(), viewProjection);
	// Vector2に格納
	Vector2 positionScreenV2(positionScreen.x, positionScreen.y - 30.0f);
	// Hpバーの座標確定
	Vector2 hpBarPosition = positionScreenV2;
	//// Hpバーのサイズ
	//hpbar_->SetSize(hpbarSize_);
	//// HPBarスプライト
	//hpbar_->SetPosition(hpBarPosition);
	//// Hpバー更新
	//hpbar_->Update(hp_);
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
	// 敵描画
	BaseObject::Draw(viewProjection);
}

///========================================================
/// Sprite描画
///========================================================
void BaseEnemy::SpriteDraw(const ViewProjection& viewProjection) {
	viewProjection;
	//// HPバー描画
	//if (!isBurst_) {
	//	if (IsInView(viewProjection)) {
	//		hpbar_->Draw();
	//	}
	//}
}

void BaseEnemy::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
	
}

void BaseEnemy::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
	//普通のパンチに攻撃されたら
	if (dynamic_cast<PunchCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyHitBackDamage*>(behavior_.get())) {
			ChangeBehavior(std::make_unique<EnemyHitBackDamage>(this));
		}
		return;
	}

	//アッパーを食らったら
	if (dynamic_cast<UpperCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyUpperDamage*>(behavior_.get())) {
			ChangeBehavior(std::make_unique<EnemyUpperDamage>(this));
		}
		return;
	}

	//止まる攻撃
	if (dynamic_cast<StopCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyStopDamage*>(behavior_.get())) {
			ChangeBehavior(std::make_unique<EnemyStopDamage>(this));
		}

		return;
	}

	//月飛ばし攻撃
	if (dynamic_cast<ThrustCollisionBox*>(other)) {

		if (!dynamic_cast<EnemyThrustDamage*>(behavior_.get())) {
			ChangeBehavior(std::make_unique<EnemyThrustDamage>(this));
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