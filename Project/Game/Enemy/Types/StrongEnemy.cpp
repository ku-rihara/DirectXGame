#include "StrongEnemy.h"
#include"Enemy/EnemyManager.h"
#include"audio/Audio.h"

///========================================================
///  初期化
///========================================================
void StrongEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
    obj3d_.reset(KetaEngine::Object3d::CreateModel("StrongEnemy.obj"));
	obj3d_->transform_.Init();
	obj3d_->transform_.SetParent(&baseTransform_);   
	obj3d_->GtModelMaterial()->GetMaterialData()->enableLighting = 2;
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
void StrongEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
	BaseEnemy::DisplaySprite(viewProjection);
}


void StrongEnemy::SpawnRenditionInit() {
   
    GetEnemyEffects()->Emit("SpawnEffectStrong");
}