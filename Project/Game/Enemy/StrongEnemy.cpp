#include "StrongEnemy.h"

///========================================================
///  初期化
///========================================================
void StrongEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
	BaseObject::CreateModel("StrongEnemy", ".obj");
	damageParm_ = 5.0f;
}

///========================================================
/// 更新
///========================================================

void StrongEnemy::Update() {
	BaseEnemy::Update();
}

///========================================================
/// 描画
///========================================================

void StrongEnemy::Draw(const ViewProjection& viewProjection) {
	BaseEnemy::Draw(viewProjection);
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
