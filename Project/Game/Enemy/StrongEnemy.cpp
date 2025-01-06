#include "StrongEnemy.h"

///========================================================
///  初期化
///========================================================
void StrongEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
	damageParm_ = 0.1f;
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
void StrongEnemy::DisplayHpBar(const ViewProjection& viewProjection) {
	BaseEnemy::DisplayHpBar(viewProjection);
}

///========================================================
/// Sprite描画
///========================================================
void StrongEnemy::SpriteDraw(const ViewProjection& viewProjection) {
	BaseEnemy::SpriteDraw(viewProjection);
}
