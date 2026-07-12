#include "EntourageEnemy.h"
#include "Utility/CollisionPush/CollisionPushUtils.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyWait.h"
#include "Enemy/Behavior/ActionBehavior/EntourageEnemyBehavior/ZakoCrawlBackwardsBehavior.h"
#include "Enemy/Behavior/ActionBehavior/EntourageEnemyBehavior/ZakoFlockBehavior.h"
#include "Enemy/Behavior/ActionBehavior/EntourageEnemyBehavior/ZakoTauntBehavior.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
#include "Light/LightingType.h"

///========================================================
///  初期化
///========================================================
void EntourageEnemy::Init(const Vector3& spawnPos) {
    pBoss_            = nullptr;
    spawnOffset_      = {};
    zakoState_        = ZakoState::Spawning;
    slotIndex_        = 0;
    slotCount_        = 1;
    parentBossName_.clear();
    isInStumblePhase_ = false;

    BaseEnemy::Init(spawnPos);
    InitAnimations();

    BaseEnemy::ChangeBehavior(std::make_unique<EnemySpawn>(this));
}

void EntourageEnemy::InitAnimations() {
    GetAnimator()->SetAnimationName(AnimationType::Wait,      "EntourageEnemyWaiting");
    GetAnimator()->SetAnimationName(AnimationType::Spawn,     "EntourageEnemySpawn");
    GetAnimator()->SetAnimationName(AnimationType::Discovery, "EntourageEnemyDiscovery");
    GetAnimator()->SetAnimationName(AnimationType::Dash,      "EntourageEnemyRun");
    GetAnimator()->SetAnimationName(AnimationType::Death,     "EnemyDeathAnimation");
    GetAnimator()->SetAnimationName(AnimationType::Taunt,     "EntourageEnemyTaunt");

    AddNormalAnimation(NormalAnimationType::StumbleBackwards, "StumbleBackwards");
    AddNormalAnimation(NormalAnimationType::CrawlBackwards,   "CrawlBackwards");

    GetAnimator()->AddDamageReactionAnimation("EnemyNormalDamage");
    GetAnimator()->AddDamageReactionAnimation("TakeUpMotion", true);
    GetAnimator()->AddDamageReactionAnimation("EntourageEnemyBoundDamage");
    GetAnimator()->AddDamageReactionAnimation("EntourageEnemyKipUp");

    auto* animObj = GetAnimator()->GetAnimationObject();
    animObj->transform_.Init();
    animObj->transform_.SetParent(&baseTransform_);
    animObj->transform_.scale_                                     = Vector3::OneVector();
    animObj->GetModelMaterial()->GetMaterialData()->enableLighting = static_cast<int32_t>(KetaEngine::LightingType::SpecularReflection);
}

///========================================================
/// 更新
///========================================================
void EntourageEnemy::Update() {
    BaseEnemy::Update();
}

///========================================================
/// スポーン後のビヘイビア生成
///========================================================
std::unique_ptr<BaseEnemyBehavior> EntourageEnemy::CreatePostSpawnBehavior() {
    if (pBoss_ && !pBoss_->GetIsDeath()) {
        zakoState_ = ZakoState::Flock;
        return std::make_unique<ZakoFlockBehavior>(this);
    }
    // ボスがいない場合は逃げる
    zakoState_ = ZakoState::CrawlBackwards;
    return std::make_unique<ZakoCrawlBackwardsBehavior>(this);
}

void EntourageEnemy::SpawnRenditionInit() {
    GetEnemyEffects()->Emit("SpawnEffect");
}

///========================================================
/// 煽り開始
///========================================================
void EntourageEnemy::StartTaunt() {
    if (zakoState_ != ZakoState::Flock) {
        return;
    }
    zakoState_ = ZakoState::Taunt;
    ChangeBehavior(std::make_unique<ZakoTauntBehavior>(this));
}

///========================================================
/// 煽り停止 → Flockに戻す
///========================================================
void EntourageEnemy::StopTaunt() {
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
void EntourageEnemy::StartFlee() {
    if (zakoState_ == ZakoState::CrawlBackwards || zakoState_ == ZakoState::Spawning) {
        return;
    }
    zakoState_ = ZakoState::CrawlBackwards;
    ChangeBehavior(std::make_unique<ZakoCrawlBackwardsBehavior>(this));
}

///========================================================
/// EntourageEnemy固有アニメーション追加
///========================================================
void EntourageEnemy::AddNormalAnimation(NormalAnimationType type, const std::string& name) {
    GetAnimator()->AddAnimationFile(name);
    normalAnimationNames_[static_cast<size_t>(type)] = name;
}

///========================================================
/// EntourageEnemy固有アニメーション再生
///========================================================
bool EntourageEnemy::PlayNormalAnimation(NormalAnimationType type, bool isLoop) {
    const std::string& name = normalAnimationNames_[static_cast<size_t>(type)];
    return GetAnimator()->PlayAnimationByName(name, isLoop);
}

///========================================================
/// ダメージRoot復帰
///========================================================
void EntourageEnemy::BackToDamageRoot() {

    // DamageReaction
    BaseEnemy::ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));

    // ダメージリアクションによってStumbleが中断された場合はフラグをリセット
    if (isInStumblePhase_) {
        isInStumblePhase_ = false;
        if (auto* anim = GetAnimator()->GetAnimationObject()) {
            anim->RemoveAnimationEndCallback("StumbleBackwards");
        }
    }

    // スポーン中の被弾で移動Behaviorが保留状態であれば、スポーン後のBehaviorを生成して移行
    if (!behaviorCtrl_.GetMoveBehavior()) {
        ChangeBehavior(CreatePostSpawnBehavior());
        return;
    }

    // CrawlBackwards状態だった場合、Stumble省略でCrawlから再開
    if (zakoState_ == ZakoState::CrawlBackwards) {
        ChangeBehavior(std::make_unique<ZakoCrawlBackwardsBehavior>(this, true));
        return;
    } else if (zakoState_ == ZakoState::Taunt) {
        ChangeBehavior(std::make_unique<ZakoTauntBehavior>(this));
        return;
    }

    // 通常のBehaviorに戻る
    GetAnimator()->ResetToWaitAnimation();
}

void EntourageEnemy::SetSlot(int32_t index, int32_t count) {
    slotIndex_ = index;
    slotCount_ = count;
}

void EntourageEnemy::OnCollisionStay(BaseCollider* other) {
    // PlayerAttackCollider 処理
    BaseEnemy::OnCollisionStay(other); 

    if (other->IsEnemy()) {
        auto* enemy = static_cast<BaseEnemy*>(other);
        if (IsAttacking() || enemy->IsAttacking()) {
            return;
        }

        // 押し戻し
        CollisionPushUtils::ApplySpherePush(
            baseTransform_.translation_,
            enemy->GetCollisionPos(),
            GetCollisionRadius(),
            enemy->GetCollisionRadius(),
            1.0f,
            0.3f);
    }
}