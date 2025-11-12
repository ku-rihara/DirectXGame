#include "EnemyManager.h"
#include "NormalEnemy.h"
#include "StrongEnemy.h"

#include "AttackEffect/AttackEffect.h"
#include "Combo/Combo.h"
#include "LockOn/LockOn.h"
#include "Spawner/EnemySpawner.h"

#include <imgui.h>

///========================================================================================
///  初期化
///========================================================================================

void EnemyManager::Init() {
    selectedEnemyType_ = "NormalEnemy";
    spawnPosition_     = {};

    /// グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    /// パーティクル初期化
    ParticleInit();
}

///========================================================================================
///  敵の生成
///========================================================================================
void EnemyManager::SpawnEnemy(const std::string& enemyType, const Vector3& position, const int32_t& groupID) {

    std::unique_ptr<BaseEnemy> enemy;

    if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]) { 
        enemy = std::make_unique<NormalEnemy>();
        enemy->SetParameter(BaseEnemy::Type::NORMAL, parameters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
    }
    if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::STRONG)]) { 
        enemy = std::make_unique<StrongEnemy>();
        enemy->SetParameter(BaseEnemy::Type::STRONG, parameters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
    }

    // 初期化とvectorに追加
    enemy->SetPlayer(pPlayer_);
    enemy->SetGameCamera(pGameCamera_);
    enemy->SetManager(this);
    enemy->SetCombo(pCombo_);
    enemy->SetAttackEffect(pAttackEffect_);
    enemy->SetGroupId(groupID);
    enemy->Init(position);
    enemies_.push_back(std::move(enemy));
}

///========================================================================================
///  更新処理
///========================================================================================
void EnemyManager::Update() {

    // 死亡した敵のインデックスを記録
    for (size_t i = 0; i < enemies_.size();) {

        if (enemies_[i]->GetType() == BaseEnemy::Type::NORMAL) {
            enemies_[i]->SetParameter(BaseEnemy::Type::NORMAL, parameters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
        } else if (enemies_[i]->GetType() == BaseEnemy::Type::STRONG) {
            enemies_[i]->SetParameter(BaseEnemy::Type::STRONG, parameters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
        }

        // 更新
        enemies_[i]->Update();

        // 死亡処理
        if (enemies_[i]->GetIsDeath()) {

            if (pEnemySpawner_) {
                pEnemySpawner_->OnEnemyDestroyed(enemies_[i]->GetGroupId());
            }

            // vectorから削除
            enemies_[i] = std::move(enemies_.back());
            enemies_.pop_back();

            CheckIsEnemiesCleared();
        } else {
            ++i;
        }
    }
    // パーティクル更新
    ParticleUpdate();
}

void EnemyManager::HpBarUpdate(const ViewProjection& viewProjection) {

    for (size_t i = 0; i < enemies_.size(); ++i) {
        enemies_[i]->DisplaySprite(viewProjection);
    }
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void EnemyManager::RegisterParams() {

    for (uint32_t i = 0; i < parameters_.size(); ++i) {
        globalParameter_->Regist(groupName_, "chaseDistance" + std::to_string(int(i + 1)), &parameters_[i].chaseDistance);
        globalParameter_->Regist(groupName_, "chaseSpeed" + std::to_string(int(i + 1)), &parameters_[i].chaseSpeed);
        globalParameter_->Regist(groupName_, "basePosY_" + std::to_string(int(i + 1)), &parameters_[i].basePosY);
        globalParameter_->Regist(groupName_, "thrustRotateSpeed" + std::to_string(int(i + 1)), &parameters_[i].thrustRotateSpeed);
        globalParameter_->Regist(groupName_, "upperGravity" + std::to_string(int(i + 1)), &parameters_[i].upperGravity);
        globalParameter_->Regist(groupName_, "upperJumpPower" + std::to_string(int(i + 1)), &parameters_[i].upperJumpPower);
        globalParameter_->Regist(groupName_, "upperFallSpeedLimit" + std::to_string(int(i + 1)), &parameters_[i].upperFallSpeedLimit);
        globalParameter_->Regist(groupName_, "hitbackRotateTime" + std::to_string(int(i + 1)), &parameters_[i].archingBackTime);
        globalParameter_->Regist(groupName_, "hitbackRotateValue" + std::to_string(int(i + 1)), &parameters_[i].archingBackValue);
        globalParameter_->Regist(groupName_, "hitbackRotateBackRatio" + std::to_string(int(i + 1)), &parameters_[i].archingBackRate);
        globalParameter_->Regist(groupName_, "blowValue_" + std::to_string(int(i + 1)), &parameters_[i].blowValue);
        globalParameter_->Regist(groupName_, "blowValueY_" + std::to_string(int(i + 1)), &parameters_[i].blowValueY);
        globalParameter_->Regist(groupName_, "blowTime" + std::to_string(int(i + 1)), &parameters_[i].blowTime);
        globalParameter_->Regist(groupName_, "blowRotateSpeed" + std::to_string(int(i + 1)), &parameters_[i].blowRotateSpeed);
        globalParameter_->Regist(groupName_, "burstTime" + std::to_string(int(i + 1)), &parameters_[i].burstTime);
        globalParameter_->Regist(groupName_, "blowGravity" + std::to_string(int(i + 1)), &parameters_[i].blowGravity);
        globalParameter_->Regist(groupName_, "initScale" + std::to_string(int(i + 1)), &parameters_[i].initScale_);
        globalParameter_->Regist(groupName_, "hpBarPosOffset" + std::to_string(int(i + 1)), &parameters_[i].hpBarPosOffset);
    }
}

void EnemyManager::DrawEnemyParamUI(BaseEnemy::Type type) {

    ImGui::DragFloat3("initScale", &parameters_[static_cast<size_t>(type)].initScale_.x, 0.01f);
    ImGui::DragFloat("ChaseSpeed", &parameters_[static_cast<size_t>(type)].chaseSpeed, 0.01f);
    ImGui::DragFloat("ChaseDistance", &parameters_[static_cast<size_t>(type)].chaseDistance, 0.01f);
    ImGui::DragFloat("basePosY", &parameters_[static_cast<size_t>(type)].basePosY, 0.01f);
    ImGui::DragFloat("thrustRotateSpeed", &parameters_[static_cast<size_t>(type)].thrustRotateSpeed, 0.01f);
    ImGui::DragFloat("upperGravity", &parameters_[static_cast<size_t>(type)].upperGravity, 0.01f);
    ImGui::DragFloat("upperJumpPowe", &parameters_[static_cast<size_t>(type)].upperJumpPower, 0.01f);
    ImGui::DragFloat("upperFallSpeedLimit", &parameters_[static_cast<size_t>(type)].upperFallSpeedLimit, 0.01f);
    ImGui::SliderAngle("hitbackRotateValue", &parameters_[static_cast<size_t>(type)].archingBackValue, -360.0f, 720.0f);
    ImGui::DragFloat("hitbackRotateBackRatio", &parameters_[static_cast<size_t>(type)].archingBackRate, 0.01f);
    ImGui::DragFloat("hitbackRotateTime", &parameters_[static_cast<size_t>(type)].archingBackTime, 0.01f);
    ImGui::DragFloat("blowValue", &parameters_[static_cast<size_t>(type)].blowValue, 0.01f);
    ImGui::DragFloat("blowValueY", &parameters_[static_cast<size_t>(type)].blowValueY, 0.01f);
    ImGui::DragFloat("blowRotateSpeed", &parameters_[static_cast<size_t>(type)].blowRotateSpeed, 0.01f);
    ImGui::DragFloat("burstTime", &parameters_[static_cast<size_t>(type)].burstTime, 0.01f);
    ImGui::DragFloat("blowGravity", &parameters_[static_cast<size_t>(type)].blowGravity, 0.01f);
    ImGui::DragFloat("blowTime", &parameters_[static_cast<size_t>(type)].blowTime, 0.01f);
    ImGui::SeparatorText("UI");
    ImGui::DragFloat2("HPBarOffsetPos", &parameters_[static_cast<size_t>(type)].hpBarPosOffset.x, 0.01f);
}

void EnemyManager::AdjustParam() {
#ifdef _DEBUG

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        for (size_t i = 0; i < static_cast<size_t>(BaseEnemy::Type::COUNT); ++i) {
            BaseEnemy::Type type = static_cast<BaseEnemy::Type>(i);
            ImGui::SeparatorText(enemyTypes_[i].c_str());
            ImGui::PushID(enemyTypes_[i].c_str());

            DrawEnemyParamUI(type);

            ImGui::PopID();
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
    ImGui::Separator();

#endif
}

///---------------------------------------------------------
/// Particle Init
///----------------------------------------------------------
void EnemyManager::ParticleInit() {

    // damage
    damageEffect[0].emitter.reset(ParticleEmitter::CreateParticlePrimitive("HitEffectCenter", PrimitiveType::Plane, 300));
    damageEffect[1].emitter.reset(ParticleEmitter::CreateParticlePrimitive("HitEffect", PrimitiveType::Plane, 300));
    damageEffect[2].emitter.reset(ParticleEmitter::CreateParticlePrimitive("HitEffectWing", PrimitiveType::Plane, 300));
    damageEffect[3].emitter.reset(ParticleEmitter::CreateParticlePrimitive("HitEffectStar", PrimitiveType::Plane, 300));

    /// death
    deathParticle_[0].emitter.reset(ParticleEmitter::CreateParticlePrimitive("EnemyDeathSmoke", PrimitiveType::Plane, 900));
    deathParticle_[1].emitter.reset(ParticleEmitter::CreateParticlePrimitive("EnemyDeathFireSmoke", PrimitiveType::Plane, 900));
    deathParticle_[2].emitter.reset(ParticleEmitter::CreateParticlePrimitive("EnemyDeathSpark", PrimitiveType::Plane, 900));
    deathParticle_[3].emitter.reset(ParticleEmitter::CreateParticlePrimitive("EnemyDeathMiniSpark", PrimitiveType::Plane, 900));

    // debri
    debriParticle_[0].emitter.reset(ParticleEmitter::CreateParticle("DebriName", "debri.obj", 500));

    // EnemySpawn
    spawnEffectNormal_[0].emitter.reset(ParticleEmitter::CreateParticlePrimitive("NormalEnemySpawnCircle", PrimitiveType::Cylinder, 200));
    spawnEffectNormal_[1].emitter.reset(ParticleEmitter::CreateParticlePrimitive("NormalEnemySpawnSpark", PrimitiveType::Plane, 500));

    spawnEffectStrong_[0].emitter.reset(ParticleEmitter::CreateParticlePrimitive("StrongEnemySpawnCircle", PrimitiveType::Cylinder, 200));
    spawnEffectStrong_[1].emitter.reset(ParticleEmitter::CreateParticlePrimitive("StrongEnemySpawnSpark", PrimitiveType::Plane, 500));

    // crack
    fallCrack_.reset(ParticleEmitter::CreateParticlePrimitive("Crack", PrimitiveType::Plane, 30));
}

///----------------------------------------------------------------------
/// Emitt Init
///----------------------------------------------------------------------

void EnemyManager::DamageEffectShot(const Vector3& pos) {
    for (uint32_t i = 0; i < damageEffect.size(); i++) {
        damageEffect[i].emitter->SetTargetPosition(pos);
        damageEffect[i].emitter->Emit();
    }
}

void EnemyManager::ThrustEmit(const Vector3& pos) {
    // ガレキパーティクル
    for (uint32_t i = 0; i < debriParticle_.size(); i++) {
        debriParticle_[i].emitter->SetTargetPosition(pos);
        debriParticle_[i].emitter->Emit();
    }
    fallCrack_->SetTargetPosition(Vector3(pos.x, 0.0f, pos.z));
    fallCrack_->Emit();
}

void EnemyManager::DeathEmit(const Vector3& pos) {
    // 死亡パーティクル
    for (uint32_t i = 0; i < deathParticle_.size(); i++) {
        deathParticle_[i].emitter->SetTargetPosition(pos);
        deathParticle_[i].emitter->Emit();
    }
}

void EnemyManager::SpawnEmitByNormalEnemy(const Vector3& pos) {
    // ガレキパーティクル
    for (uint32_t i = 0; i < spawnEffectNormal_.size(); i++) {
        spawnEffectNormal_[i].emitter->SetTargetPosition(pos);
        spawnEffectNormal_[i].emitter->Emit();
    }
}

void EnemyManager::SpawnEmitByStrongEnemy(const Vector3& pos) {
    // ガレキパーティクル
    for (uint32_t i = 0; i < spawnEffectStrong_.size(); i++) {
        spawnEffectStrong_[i].emitter->SetTargetPosition(pos);
        spawnEffectStrong_[i].emitter->Emit();
    }
}

///---------------------------------------------------------
/// Particle Update
///----------------------------------------------------------
void EnemyManager::ParticleUpdate() {
    // ダメージパーティクル
    for (uint32_t i = 0; i < damageEffect.size(); i++) {
        damageEffect[i].emitter->Update();
    }

    // スポーンパーティクル
    for (uint32_t i = 0; i < spawnEffectNormal_.size(); i++) {
        spawnEffectNormal_[i].emitter->Update();
    }

    // スポーンパーティクル(strong)
    for (uint32_t i = 0; i < spawnEffectStrong_.size(); i++) {
        spawnEffectStrong_[i].emitter->Update();
    }

    // ひび
    fallCrack_->Update();

    // 死亡パーティクル
    for (uint32_t i = 0; i < deathParticle_.size(); i++) {
        deathParticle_[i].emitter->Update();
    }

    // ガレキパーティクル
    for (uint32_t i = 0; i < debriParticle_.size(); i++) {
        debriParticle_[i].emitter->Update();
    }
}

///------------------------------------------------------------------------------------------------
/// クラスセット
///------------------------------------------------------------------------------------------------

void EnemyManager::SetPlayer(Player* player) {
    pPlayer_ = player;
}

void EnemyManager::SetCombo(Combo* lockOn) {
    pCombo_ = lockOn;
}

void EnemyManager::SetGameCamera(GameCamera* gameCamera) {
    pGameCamera_ = gameCamera;
}

void EnemyManager::SetEnemySpawner(EnemySpawner* enemySpawner) {
    pEnemySpawner_ = enemySpawner;
}

void EnemyManager::SetAttackEffect(AttackEffect* effect) {
    pAttackEffect_ = effect;
}

void EnemyManager::CheckIsEnemiesCleared() {
    areAllEnemiesCleared_ = enemies_.empty();
}