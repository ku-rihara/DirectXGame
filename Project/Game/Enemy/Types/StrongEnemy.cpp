#include "StrongEnemy.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "Light/LightingType.h"

///========================================================
///  初期化
///========================================================
void StrongEnemy::Init(const Vector3& spawnPos) {
    BaseEnemy::Init(spawnPos);

    // アニメーション名を設定
    SetAnimationName(AnimationType::Wait, "NormalEnemyWaiting");
    SetAnimationName(AnimationType::Spawn, "NormalEnemySpawn");
    SetAnimationName(AnimationType::Discovery, "NormalEnemyDiscovery");
    SetAnimationName(AnimationType::Dash, "NormalEnemyRun");
    SetAnimationName(AnimationType::Attack, "NormalEnemyAttack");

    // ダメージリアクション用アニメーションを追加
    AddDamageReactionAnimation("EnemyNormalDamage");

    objAnimation_->transform_.Init();
    objAnimation_->transform_.SetParent(&baseTransform_);
    objAnimation_->transform_.scale_ = Vector3::OneVector();
    objAnimation_->GetModelMaterial()->GetMaterialData()->enableLighting = static_cast<int32_t>(KetaEngine::LightingType::SpecularReflection);

    BaseEnemy::ChangeBehavior(std::make_unique<EnemySpawn>(this));
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
