#include "NormalEnemy.h"
#include"Enemy/EnemyManager.h"
#include"audio/Audio.h"

///========================================================
///  初期化
///========================================================
void NormalEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
	// モデルセット
	/*damageParam_ = 30.0f;*/
    obj3d_.reset(KetaEngine::Object3d::CreateModel("NormalEnemy.obj"));
    obj3d_->transform_.Init();
    obj3d_->transform_.SetParent(&baseTransform_);
    obj3d_->material_.GetMaterialData()->enableLighting = 2;
}

///========================================================
/// 更新
///========================================================

void NormalEnemy::Update() {
	BaseEnemy::Update();
}


///========================================================
/// HpBar表示
///========================================================
void NormalEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
	BaseEnemy::DisplaySprite(viewProjection);

}

void NormalEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffect");
}