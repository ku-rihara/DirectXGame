#include "StrongEnemy.h"

///========================================================
///  初期化
///========================================================
void StrongEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
	damageParm_ = 5.0f;
	bodyObj_.reset(Object3d::CreateModel("StrongEnemy", ".obj"));
	bodyTransform_.Init();
	bodyTransform_.SetParent(&transform_);
}

///========================================================
/// 更新
///========================================================

void StrongEnemy::Update() {
	BaseEnemy::Update();
	bodyTransform_.UpdateMatrix();
}

///========================================================
/// 描画
///========================================================

void StrongEnemy::Draw(const ViewProjection& viewProjection) {
	BaseEnemy::Draw(viewProjection);
	bodyObj_->Draw(bodyTransform_, viewProjection);
}

///========================================================
/// HpBar表示
///========================================================
void StrongEnemy::DisplaySprite(const ViewProjection& viewProjection) {
	BaseEnemy::DisplaySprite(viewProjection);
}

///========================================================
/// Sprite描画
///========================================================
void StrongEnemy::SpriteDraw(const ViewProjection& viewProjection) {
	BaseEnemy::SpriteDraw(viewProjection);
}
