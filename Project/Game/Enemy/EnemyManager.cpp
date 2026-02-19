#include "EnemyManager.h"
// Types
#include "Types/NormalEnemy.h"
#include "Types/StrongEnemy.h"
// Combo
#include "Combo/Combo.h"
// LockOn
#include "LockOn/LockOn.h"
// Spawner
#include "Spawner/EnemySpawner.h"
// imGui
#include <imgui.h>

///========================================================================================
///  初期化
///========================================================================================

void EnemyManager::Init() {
    selectedEnemyType_ = "NormalEnemy";
    spawnPosition_     = {};

    // ダメージリアクション初期化
    damageReactionController_ = std::make_unique<EnemyDamageReactionController>();
    damageReactionController_->Init();

    /// グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    /// パーティクル初期化
    ParticleInit();
}

///========================================================================================
///  敵の生成
///========================================================================================
void EnemyManager::SpawnEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID) {

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
    enemy->SetKillCounter(pKillCounter_);
    enemy->SetGroupId(groupID);
    enemy->Init(position);

    // エディター用にアニメーションリストを更新
    UpdateAvailableAnimationsForEditor(enemy.get());

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

void EnemyManager::HpBarUpdate(const KetaEngine::ViewProjection& viewProjection) {

    for (size_t i = 0; i < enemies_.size(); ++i) {
        enemies_[i]->DisplaySprite(viewProjection);
    }
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void EnemyManager::RegisterParams() {

    for (uint32_t i = 0; i < parameters_.size(); ++i) {
        const std::string& indexString = std::to_string(static_cast<int>(i + 1));

        // 基本パラメータ
        globalParameter_->Regist(groupName_, "basePosY_" + indexString, &parameters_[i].basePosY);
        globalParameter_->Regist(groupName_, "burstTime" + indexString, &parameters_[i].burstTime);
        globalParameter_->Regist(groupName_, "initScale" + indexString, &parameters_[i].baseScale_);
        globalParameter_->Regist(groupName_, "collisionSize" + indexString, &parameters_[i].collisionSize);
        globalParameter_->Regist(groupName_, "collisionOffset" + indexString, &parameters_[i].collisionOffset);
        globalParameter_->Regist(groupName_, "hpBarPosOffset" + indexString, &parameters_[i].hpBarPosOffset);

        // 逃走パラメータ
        globalParameter_->Regist(groupName_, "escapeTime" + indexString, &parameters_[i].escapeTime);
        globalParameter_->Regist(groupName_, "escapeSpeed" + indexString, &parameters_[i].escapeSpeed);
        globalParameter_->Regist(groupName_, "escapeDistance" + indexString, &parameters_[i].escapeDistance);

        // 死亡パラメータ
        globalParameter_->Regist(groupName_, "deathBlowValue" + indexString, &parameters_[i].deathBlowValue);
        globalParameter_->Regist(groupName_, "deathBlowValueY" + indexString, &parameters_[i].deathBlowValueY);
        globalParameter_->Regist(groupName_, "deathGravity" + indexString, &parameters_[i].deathGravity);
        globalParameter_->Regist(groupName_, "deathRotateSpeed" + indexString, &parameters_[i].deathRotateSpeed);
        globalParameter_->Regist(groupName_, "deathBurstTime" + indexString, &parameters_[i].deathBurstTime);
    }
}

void EnemyManager::DrawEnemyParamUI(BaseEnemy::Type type) {
    const size_t typeIndex = static_cast<size_t>(type);

    ImGui::SeparatorText("基本パラメータ");
    ImGui::DragFloat3("initScale", &parameters_[typeIndex].baseScale_.x, 0.01f);
    ImGui::DragFloat3("collisionSize", &parameters_[typeIndex].collisionSize.x, 0.1f, 0.1f, 20.0f);
    ImGui::DragFloat3("collisionOffset", &parameters_[typeIndex].collisionOffset.x, 0.1f);
    ImGui::DragFloat("basePosY", &parameters_[typeIndex].basePosY, 0.01f);
    ImGui::DragFloat("burstTime", &parameters_[typeIndex].burstTime, 0.01f);

    ImGui::SeparatorText("逃走パラメータ");
    ImGui::DragFloat("逃避開始距離", &parameters_[typeIndex].escapeDistance, 0.01f, 0.0f);
    ImGui::DragFloat("逃走時間", &parameters_[typeIndex].escapeTime, 0.1f, 0.0f);
    ImGui::DragFloat("逃走速度", &parameters_[typeIndex].escapeSpeed, 0.01f, 0.0f);

    ImGui::SeparatorText("死亡パラメータ");
    ImGui::DragFloat("DeathBlowValue", &parameters_[typeIndex].deathBlowValue, 0.1f);
    ImGui::DragFloat("DeathBlowValueY", &parameters_[typeIndex].deathBlowValueY, 0.1f);
    ImGui::DragFloat("DeathGravity", &parameters_[typeIndex].deathGravity, 0.1f);
    ImGui::DragFloat("DeathRotateSpeed", &parameters_[typeIndex].deathRotateSpeed, 0.01f);
    ImGui::DragFloat("DeathBurstTime", &parameters_[typeIndex].deathBurstTime, 0.01f);

    ImGui::SeparatorText("スプライト関連");
    ImGui::DragFloat2("HPBarOffsetPos", &parameters_[typeIndex].hpBarPosOffset.x, 0.01f);
}
void EnemyManager::DebugEnemySpawn() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Enemy Spawn")) {
        ImGui::DragFloat3("SpawnPosition", &spawnPosition_.x, 0.1f);

        const char* enemyType[] = {"Normal", "Strong"};
        ImGui::Combo("SpawnType", &selectedEnemyTypeIndex_, enemyType, IM_ARRAYSIZE(enemyType));

        if (ImGui::Button("Spawn Enemy")) {
            SpawnEnemy(enemyTypes_[selectedEnemyTypeIndex_], spawnPosition_, 0);
        }
    }
    ImGui::Separator();
#endif
}

void EnemyManager::AdjustParam() {
#ifdef _DEBUG

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        // 敵のパラメータ編集
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

void EnemyManager::DamageReactionCreate() {
    damageReactionController_->EditorUpdate();
}

///---------------------------------------------------------
/// Particle Init
///----------------------------------------------------------
void EnemyManager::ParticleInit() {

    /// death
    deathParticle_[0].emitter.reset(KetaEngine::ParticleEmitter::CreateParticlePrimitive("EnemyDeathSmoke", PrimitiveType::Plane, 900));
    deathParticle_[1].emitter.reset(KetaEngine::ParticleEmitter::CreateParticlePrimitive("EnemyDeathFireSmoke", PrimitiveType::Plane, 900));
    deathParticle_[2].emitter.reset(KetaEngine::ParticleEmitter::CreateParticlePrimitive("EnemyDeathSpark", PrimitiveType::Plane, 900));
    deathParticle_[3].emitter.reset(KetaEngine::ParticleEmitter::CreateParticlePrimitive("EnemyDeathMiniSpark", PrimitiveType::Plane, 900));

    // ガレキ
    debriParticle_[0].emitter.reset(KetaEngine::ParticleEmitter::CreateParticle("DebriName", "Player/debri.obj", 500));

    // crack
    fallCrack_.reset(KetaEngine::ParticleEmitter::CreateParticlePrimitive("Crack", PrimitiveType::Plane, 30));
}

///----------------------------------------------------------------------
/// Emit Init
///----------------------------------------------------------------------

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

///---------------------------------------------------------
/// Particle Update
///----------------------------------------------------------
void EnemyManager::ParticleUpdate() {

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

void EnemyManager::SetKillCounter(KillCounter* killCounter) {
    pKillCounter_ = killCounter;
}

void EnemyManager::SetGameCamera(GameCamera* gameCamera) {
    pGameCamera_ = gameCamera;
}

void EnemyManager::SetEnemySpawner(EnemySpawner* enemySpawner) {
    pEnemySpawner_ = enemySpawner;
}

void EnemyManager::UpdateAvailableAnimationsForEditor(BaseEnemy* enemy) {
    if (!enemy) {
        return;
    }

    // ダメージリアクション用アニメーションリストを取得
    const auto& animeNames = enemy->GetDamageReactionAnimationNames();

    // エディター用に利用可能なアニメーションリストを設定
    if (!animeNames.empty()) {
        damageReactionController_->SetAvailableAnimations(animeNames);
    }
}

void EnemyManager::CheckIsEnemiesCleared() {
    areAllEnemiesCleared_ = enemies_.empty();
}

bool EnemyManager::IsAnyEnemyInAnticipation() const {
    for (const auto& enemy : enemies_) {
        if (enemy->IsInAnticipation()) {
            return true;
        }
    }
    return false;
}