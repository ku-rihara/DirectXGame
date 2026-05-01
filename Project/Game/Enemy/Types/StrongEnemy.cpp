#include "StrongEnemy.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyChase.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "Enemy/Behavior/ActionBehavior/StrongEnemyBehavior/StrongEnemyTauntBehavior.h"
#include "Light/LightingType.h"

void StrongEnemy::Init(const Vector3& spawnPos) {
    BaseEnemy::Init(spawnPos);

    SetAnimationName(AnimationType::Wait, "StrongEnemyWaiting");
    SetAnimationName(AnimationType::Spawn, "StrongEnemySpawn");
    SetAnimationName(AnimationType::Discovery, "NormalEnemyDiscovery");
    SetAnimationName(AnimationType::Dash, "StrongEnemyRun");
    SetAnimationName(AnimationType::Attack, "NormalEnemyAttack");
    SetAnimationName(AnimationType::Death, "EnemyDeathAnimation");
    SetAnimationName(AnimationType::Taunt, "StrongEnemyTaunt");

    AddDamageReactionAnimation("EnemyNormalDamage");
    AddDamageReactionAnimation("TakeUpMotion", true);
    AddDamageReactionAnimation("NormalEnemyBoundDamage");
    AddDamageReactionAnimation("NormalEnemyKipUp");

    objAnimation_->transform_.Init();
    objAnimation_->transform_.SetParent(&baseTransform_);
    objAnimation_->transform_.scale_                                     = Vector3::OneVector();
    objAnimation_->GetModelMaterial()->GetMaterialData()->enableLighting = static_cast<int32_t>(KetaEngine::LightingType::SpecularReflection);

    BaseEnemy::ChangeBehavior(std::make_unique<EnemySpawn>(this));
}

void StrongEnemy::Update() {
    BaseEnemy::Update();
}

void StrongEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffectStrong");
}

void StrongEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
    BaseEnemy::DisplaySprite(viewProjection);
}

void StrongEnemy::StartTaunt() {
    if (isTaunting_)
        return;
    isTaunting_ = true;
    ChangeBehavior(std::make_unique<StrongEnemyTauntBehavior>(this));
}

void StrongEnemy::StopTaunt() {
    if (!isTaunting_)
        return;
    isTaunting_ = false;
    ChangeBehavior(std::make_unique<EnemyChase>(this));
}