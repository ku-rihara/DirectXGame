#include "NormalEnemy.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyWait.h"
#include "Enemy/Behavior/ActionBehavior/NormalEnemyBehavior/ZakoFlockBehavior.h"
#include "Enemy/Behavior/ActionBehavior/NormalEnemyBehavior/ZakoTauntBehavior.h"
#include "Enemy/Behavior/ActionBehavior/NormalEnemyBehavior/ZakoFleeBehavior.h"
#include "Light/LightingType.h"

///========================================================
///  初期化
///========================================================
void NormalEnemy::Init(const Vector3& spawnPos) {
    BaseEnemy::Init(spawnPos);

    // アニメーション名を設定
    SetAnimationName(AnimationType::Wait,           "NormalEnemyWaiting");
    SetAnimationName(AnimationType::Spawn,          "NormalEnemySpawn");
    SetAnimationName(AnimationType::Discovery,      "NormalEnemyDiscovery");
    SetAnimationName(AnimationType::Dash,           "NormalEnemyRun");
    SetAnimationName(AnimationType::Attack,         "NormalEnemyAttack");
    SetAnimationName(AnimationType::Death,          "EnemyDeathAnimation");
    SetAnimationName(AnimationType::Taunt,          "NormalEnemyTaunt");

    // ダメージリアクション用アニメーションを追加
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

///========================================================
/// 更新
///========================================================
void NormalEnemy::Update() {
    BaseEnemy::Update();
}

///========================================================
/// スポーン後のビヘイビア生成
/// ボスが設定されていれば群れ行動、なければ通常待機
///========================================================
std::unique_ptr<BaseEnemyBehavior> NormalEnemy::CreatePostSpawnBehavior() {
    if (pBoss_ && !pBoss_->GetIsDeath()) {
        zakoState_ = ZakoState::Flock;
        return std::make_unique<ZakoFlockBehavior>(this);
    }
    // ボスなし or ボス撃破済み → 即逃走
    zakoState_ = ZakoState::Flee;
    return std::make_unique<ZakoFleeBehavior>(this);
}

void NormalEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffect");
}

///========================================================
/// 煽り開始
///========================================================
void NormalEnemy::StartTaunt() {
    if (zakoState_ != ZakoState::Flock) {
        return;
    }
    zakoState_ = ZakoState::Taunt;
    ChangeBehavior(std::make_unique<ZakoTauntBehavior>(this));
}

///========================================================
/// 煽り停止 → Flockに戻す
///========================================================
void NormalEnemy::StopTaunt() {
    if (zakoState_ != ZakoState::Taunt) {
        return;
    }
    zakoState_ = ZakoState::Flock;
    if (pBoss_ && !pBoss_->GetIsDeath()) {
        ChangeBehavior(std::make_unique<ZakoFlockBehavior>(this));
    } else {
        ChangeBehavior(std::make_unique<EnemyWait>(this));
    }
}

///========================================================
/// 逃走開始
///========================================================
void NormalEnemy::StartFlee() {
    if (zakoState_ == ZakoState::Flee || zakoState_ == ZakoState::Spawning) {
        return;
    }
    zakoState_ = ZakoState::Flee;
    ChangeBehavior(std::make_unique<ZakoFleeBehavior>(this));
}

///========================================================
/// HpBar表示
///========================================================
void NormalEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
    BaseEnemy::DisplaySprite(viewProjection);
}
