#include "NormalEnemy.h"
#include "utility/CollisionPush/CollisionPushUtils.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyWait.h"
#include "Enemy/Behavior/ActionBehavior/NormalEnemyBehavior/ZakoCrawlBackwardsBehavior.h"
#include "Enemy/Behavior/ActionBehavior/NormalEnemyBehavior/ZakoFlockBehavior.h"
#include "Enemy/Behavior/ActionBehavior/NormalEnemyBehavior/ZakoTauntBehavior.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
#include "Light/LightingType.h"

///========================================================
///  初期化
///========================================================
void NormalEnemy::Init(const Vector3& spawnPos) {
    // プール再利用時のために NormalEnemy 固有状態をリセット
    pBoss_            = nullptr;
    spawnOffset_      = {};
    zakoState_        = ZakoState::Spawning;
    slotIndex_        = 0;
    slotCount_        = 1;
    parentBossName_.clear();
    isInStumblePhase_ = false;

    BaseEnemy::Init(spawnPos);

    // アニメーション名を設定
    SetAnimationName(AnimationType::Wait, "NormalEnemyWaiting");
    SetAnimationName(AnimationType::Spawn, "NormalEnemySpawn");
    SetAnimationName(AnimationType::Discovery, "NormalEnemyDiscovery");
    SetAnimationName(AnimationType::Dash, "NormalEnemyRun");
    SetAnimationName(AnimationType::Death, "EnemyDeathAnimation");
    SetAnimationName(AnimationType::Taunt, "NormalEnemyTaunt");

    // NormalEnemy固有アニメーション
    AddNormalAnimation(NormalAnimationType::StumbleBackwards, "StumbleBackwards");
    AddNormalAnimation(NormalAnimationType::CrawlBackwards, "CrawlBackwards");

    // ダメージリアクション用アニメーションを追加
    AddDamageReactionAnimation("EnemyNormalDamage");
    AddDamageReactionAnimation("TakeUpMotion", true);
    AddDamageReactionAnimation("NormalEnemyBoundDamage");
    AddDamageReactionAnimation("NormalEnemyKipUp");

    // アニメーションの初期化
    objAnimation_->transform_.Init();
    objAnimation_->transform_.SetParent(&baseTransform_);
    objAnimation_->transform_.scale_                                     = Vector3::OneVector();
    objAnimation_->GetModelMaterial()->GetMaterialData()->enableLighting = static_cast<int32_t>(KetaEngine::LightingType::SpecularReflection);
    
    // スポーン後の行動を生成
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
///========================================================
std::unique_ptr<BaseEnemyBehavior> NormalEnemy::CreatePostSpawnBehavior() {
    if (pBoss_ && !pBoss_->GetIsDeath()) {
        zakoState_ = ZakoState::Flock;
        return std::make_unique<ZakoFlockBehavior>(this);
    }
    // ボスがいない場合は逃げる
    zakoState_ = ZakoState::CrawlBackwards;
    return std::make_unique<ZakoCrawlBackwardsBehavior>(this);
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
    if (zakoState_ == ZakoState::CrawlBackwards || zakoState_ == ZakoState::Spawning) {
        return;
    }
    zakoState_ = ZakoState::CrawlBackwards;
    ChangeBehavior(std::make_unique<ZakoCrawlBackwardsBehavior>(this));
}

///========================================================
/// NormalEnemy固有アニメーション追加
///========================================================
void NormalEnemy::AddNormalAnimation(NormalAnimationType type, const std::string& name) {
    objAnimation_->Add(GetModelFolder() + name + ".gltf");
    normalAnimationNames_[static_cast<size_t>(type)] = name;
}

///========================================================
/// NormalEnemy固有アニメーション再生
///========================================================
bool NormalEnemy::PlayNormalAnimation(NormalAnimationType type, bool isLoop) {
    const std::string& name = normalAnimationNames_[static_cast<size_t>(type)];
    return PlayAnimationByName(name, isLoop);
}

///========================================================
/// ダメージRoot復帰
///========================================================
void NormalEnemy::BackToDamageRoot() {

    // DamageReaction
    BaseEnemy::ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(this));

    // ダメージリアクションによってStumbleが中断された場合はフラグをリセット
    if (isInStumblePhase_) {
        isInStumblePhase_ = false;
        if (auto* anim = GetAnimationObject()) {
            anim->RemoveAnimationEndCallback("StumbleBackwards");
        }
    }

    // CrawlBackwards状態だった場合: Stumble省略でCrawlから再開
    if (zakoState_ == ZakoState::CrawlBackwards) {
        ChangeBehavior(std::make_unique<ZakoCrawlBackwardsBehavior>(this, true));
        return;
    } else if (zakoState_ == ZakoState::Taunt) {
        ChangeBehavior(std::make_unique<ZakoTauntBehavior>(this));
        return;
    }

    // 通常のBehaviorに戻る
    BaseEnemy::ResetToWaitAnimation();
}

void NormalEnemy::SetSlot(int32_t index, int32_t count) {
    slotIndex_ = index;
    slotCount_ = count;
}

void NormalEnemy::OnCollisionStay(BaseCollider* other) {
    // PlayerAttackCollider 処理
    BaseEnemy::OnCollisionStay(other); 

    if (BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(other)) {
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