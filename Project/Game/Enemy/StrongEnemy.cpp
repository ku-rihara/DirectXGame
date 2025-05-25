#include "StrongEnemy.h"
#include"Enemy/EnemyManager.h"
#include"audio/Audio.h"

///========================================================
///  初期化
///========================================================
void StrongEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
	damageParm_ = 20.0f;
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


void StrongEnemy::SpawnRenditionInit() {
    // ガレキパーティクル
    pEnemyManager_->SpawnEmitByStrongEnemy(GetWorldPosition());
   /* Audio::GetInstance()->PlayWave(thurstSound_, 0.2f);*/
}