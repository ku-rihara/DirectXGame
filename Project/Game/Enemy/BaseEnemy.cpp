#include "BaseEnemy.h"
#include"Matrix4x4.h"


//uint32_t BaseEnemy::nextSerialNum_ = 0;
BaseEnemy::BaseEnemy() {
	//// シリアル番号を振る
	//serialNum_ = nextSerialNum_;
	//// 次の番号を加算
	//++nextSerialNum_;
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
}

///========================================================
/// 更新
///========================================================
void BaseEnemy::Update() {

	//
	//// 体力がなくなったら死亡
	//if (hp_ <= 0 && !isBurst_) {
	//	behaviorRequest_ = Behavior::kDeath;
	//	Audio::GetInstance()->PlayWave(deathSound_);
	//}
	//// ダメージ受けてたら赤色
	//if (behavior_ == Behavior::kDamage) {
	//	objColor_.SetColor(Vector4(0.9f, 0, 0, 0.9f));
	//} else {
	//	objColor_.SetColor(Vector4(1, 1, 1, 1));
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