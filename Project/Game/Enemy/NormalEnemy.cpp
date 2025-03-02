#include "NormalEnemy.h"

///========================================================
///  初期化
///========================================================
void NormalEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
	///// モデルセット
	//BaseObject::CreateModel("NormalEnemy", ".obj");
	damageParm_ = 30.0f;
	bodyObj_.reset(Object3d::CreateModel("NormalEnemy", ".obj"));
	bodyTransform_.Init();
	bodyTransform_.SetParent(&transform_);
}

///========================================================
/// 更新
///========================================================

void NormalEnemy::Update() {
	BaseEnemy::Update();
	bodyTransform_.UpdateMatrix();
}

///========================================================
/// 描画
///========================================================

void NormalEnemy::Draw(const ViewProjection& viewProjection) {
	BaseEnemy::Draw(viewProjection);
	bodyObj_->Draw(bodyTransform_, viewProjection);
}

///========================================================
/// HpBar表示
///========================================================
void NormalEnemy::DisplaySprite(const ViewProjection& viewProjection) {
	BaseEnemy::DisplaySprite(viewProjection);

}

///========================================================
/// Sprite描画
///========================================================
void NormalEnemy::SpriteDraw(const ViewProjection& viewProjection) {
	BaseEnemy::SpriteDraw(viewProjection);
}
