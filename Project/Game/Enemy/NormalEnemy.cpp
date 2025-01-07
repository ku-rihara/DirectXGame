#include "NormalEnemy.h"

///========================================================
///  初期化
///========================================================
void NormalEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
	/// モデルセット
	BaseObject::CreateModel("NormalEnemy", ".obj");
	damageParm_ = 0.2f;
}

///========================================================
/// 更新
///========================================================

void NormalEnemy::Update() {
	BaseEnemy::Update();
}

///========================================================
/// 描画
///========================================================

void NormalEnemy::Draw(const ViewProjection& viewProjection) {
	BaseEnemy::Draw(viewProjection);
}

///========================================================
/// HpBar表示
///========================================================
void NormalEnemy::DisplayHpBar(const ViewProjection& viewProjection) {
	BaseEnemy::DisplayHpBar(viewProjection);
}

///========================================================
/// Sprite描画
///========================================================
void NormalEnemy::SpriteDraw(const ViewProjection& viewProjection) {
	BaseEnemy::SpriteDraw(viewProjection);
}
