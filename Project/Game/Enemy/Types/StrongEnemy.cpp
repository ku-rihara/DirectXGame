#include "StrongEnemy.h"
#include "../Behavior/AttackStrategy/StrongEnemyAttackStrategy.h"
#include "audio/Audio.h"
#include "Enemy/EnemyManager.h"
#include "Frame/Frame.h"

///========================================================
///  初期化
///========================================================
void StrongEnemy::Init(const Vector3& spawnPos) {
    BaseEnemy::Init(spawnPos);
    obj3d_.reset(KetaEngine::Object3d::CreateModel("StrongEnemy.obj"));
    obj3d_->transform_.Init();
    obj3d_->transform_.SetParent(&baseTransform_);
    obj3d_->GetModelMaterial()->GetMaterialData()->enableLighting = 2;

    // 攻撃戦略を設定
    SetAttackStrategy(std::make_unique<StrongEnemyAttackStrategy>(this));
}

///========================================================
/// 更新
///========================================================
void StrongEnemy::Update() {
    BaseEnemy::Update();
}

void StrongEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffectStrong");
}

///========================================================
/// HpBar表示
///========================================================
void StrongEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
    BaseEnemy::DisplaySprite(viewProjection);
}
