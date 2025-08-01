#include "NormalEnemy.h"
#include"Enemy/EnemyManager.h"
#include"audio/Audio.h"

///========================================================
///  初期化
///========================================================
void NormalEnemy::Init(const Vector3& spownPos) {
	BaseEnemy::Init(spownPos);
	// モデルセット
	damageParm_ = 30.0f;
    obj3d_.reset(Object3d::CreateModel("NormalEnemy.obj"));
    obj3d_->transform_.Init();
    obj3d_->transform_.SetParent(&baseTransform_);
    obj3d_->material_.materialData_->enableLighting = 2;
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
    viewProjection;
	//BaseEnemy::Draw(viewProjection);
	//bodyObj_->Draw(bodyTransform_, viewProjection);
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

void NormalEnemy::SpawnRenditionInit() {
    // ガレキパーティクル
    pEnemyManager_->SpawnEmitByNormalEnemy(baseTransform_.translation_);
    /* Audio::GetInstance()->PlayWave(thurstSound_, 0.2f);*/
}