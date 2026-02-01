#include "NormalEnemy.h"
#include "../Behavior/AttackStrategy/NormalEnemyAttackStrategy.h"
#include "audio/Audio.h"
#include "Enemy/EnemyManager.h"
#include "Frame/Frame.h"

///========================================================
///  初期化
///========================================================
void NormalEnemy::Init(const Vector3& spownPos) {
    BaseEnemy::Init(spownPos);

    // アニメーション名を設定
    // アニメーション名を設定
    SetAnimationName(AnimationType::Wait, "NormalEnemyWaiting");
    SetAnimationName(AnimationType::Spawn, "NormalEnemySpawn");
    SetAnimationName(AnimationType::PreDash, "NormalEnemyDiscovery");
    SetAnimationName(AnimationType::Dash, "NormalEnemyRun");
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
    SetAttackStrategy(std::make_unique<NormalEnemyAttackStrategy>(this));
}

///========================================================
/// 更新
///========================================================
void NormalEnemy::Update() {
    BaseEnemy::Update();
}

void NormalEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffect");
}

///========================================================
/// HpBar表示
///========================================================
void NormalEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
    BaseEnemy::DisplaySprite(viewProjection);
}

