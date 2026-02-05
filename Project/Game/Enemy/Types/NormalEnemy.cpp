#include "NormalEnemy.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"

///========================================================
///  初期化
///========================================================
void NormalEnemy::Init(const Vector3& spawnPos) {
    BaseEnemy::Init(spawnPos);

    // アニメーション名を設定
    SetAnimationName(AnimationType::Wait, "NormalEnemyWaiting");
    SetAnimationName(AnimationType::Spawn, "NormalEnemySpawn");
    SetAnimationName(AnimationType::AttackAnticipation, "NormalEnemyDiscovery");
    SetAnimationName(AnimationType::Dash, "NormalEnemyRun");
    SetAnimationName(AnimationType::Attack, "NormalEnemyAttack");

    // アニメーションオブジェクトの作成
    objAnimation_.reset(KetaEngine::Object3DAnimation::CreateModel(GetAnimationName(AnimationType::Wait) + ".gltf"));
    objAnimation_->Init();
    objAnimation_->Add(GetAnimationName(AnimationType::Spawn) + ".gltf");
    objAnimation_->Add(GetAnimationName(AnimationType::AttackAnticipation) + ".gltf");
    objAnimation_->Add(GetAnimationName(AnimationType::Dash) + ".gltf");
    objAnimation_->Add(GetAnimationName(AnimationType::Attack) + ".gltf");
    objAnimation_->transform_.Init();
    objAnimation_->transform_.SetParent(&baseTransform_);
    objAnimation_->transform_.scale_                                     = Vector3::OneVector();
    objAnimation_->GetModelMaterial()->GetMaterialData()->enableLighting = 3;

    BaseEnemy::ChangeBehavior(std::make_unique<EnemySpawn>(this));
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

void NormalEnemy::OnPlayerApproachAction() {
}

void NormalEnemy::OnPlayerDistantAction() {

    //  // 攻撃範囲より遠い場合はWaitに戻る
    // if (distance_ > param.attackRangeMax) {
    //    pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
    //    return;
    //}
}

///========================================================
/// HpBar表示
///========================================================
void NormalEnemy::DisplaySprite(const KetaEngine::ViewProjection& viewProjection) {
    BaseEnemy::DisplaySprite(viewProjection);
}
