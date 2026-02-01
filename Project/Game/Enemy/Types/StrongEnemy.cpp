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

    // アニメーション名を設定
    SetAnimationName(AnimationType::Wait, "NormalEnemyWaiting");
    SetAnimationName(AnimationType::Spawn, "NormalEnemySpawn");
    SetAnimationName(AnimationType::PreDash, "BoxerEnemyPreDashMotion");
    SetAnimationName(AnimationType::Dash, "BoxerEnemyDash");
    SetAnimationName(AnimationType::Attack, "NormalEnemyAttack");

    // アニメーションオブジェクトの作成
    objAnimation_.reset(KetaEngine::Object3DAnimation::CreateModel(GetAnimationName(AnimationType::Wait) + ".gltf"));
    objAnimation_->Init();
    objAnimation_->Add(GetAnimationName(AnimationType::Spawn) + ".gltf");
    objAnimation_->Add(GetAnimationName(AnimationType::PreDash) + ".gltf");
    objAnimation_->Add(GetAnimationName(AnimationType::PreDash) + ".gltf");
    objAnimation_->Add(GetAnimationName(AnimationType::Attack) + ".gltf");
    objAnimation_->Add(GetAnimationName(AnimationType::Dash) + ".gltf");
    objAnimation_->transform_.Init();
    objAnimation_->transform_.SetParent(&baseTransform_);
    objAnimation_->transform_.scale_ = Vector3::OneVector();
    objAnimation_->GetModelMaterial()->GetMaterialData()->enableLighting = 3;

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
