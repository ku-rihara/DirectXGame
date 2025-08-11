#include "StrongEnemy.h"
#include"Enemy/EnemyManager.h"
#include"audio/Audio.h"

///========================================================
///  初期化
///========================================================
void StrongEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
	/*damageParam_ = 20.0f;*/
    obj3d_.reset(Object3d::CreateModel("StrongEnemy.obj"));
	obj3d_->transform_.Init();
	obj3d_->transform_.SetParent(&baseTransform_);   
	obj3d_->material_.materialData_->enableLighting = 2;
}

///========================================================
/// 更新
///========================================================

void StrongEnemy::Update() {
	BaseEnemy::Update();
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
    pEnemyManager_->SpawnEmitByStrongEnemy(baseTransform_.translation_);
}