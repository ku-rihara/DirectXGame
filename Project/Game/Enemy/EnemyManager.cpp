
#include "EnemyManager.h"
#include "base/TextureManager.h"
#include "NormalEnemy.h"
#include "StrongEnemy.h"

#include "Combo/Combo.h"
#include "Frame/Frame.h"
#include "LockOn/LockOn.h"
#include "Spawner/EnemySpawner.h"

#include <format>
#include <fstream>
#include <imgui.h>

///========================================================================================
///  初期化
///========================================================================================

void EnemyManager::Init() {
    selectedEnemyType_ = "NormalEnemy";
    spownPosition_     = {};

    spownNum_ = 1;

    ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    AddParamGroup();
    ApplyGlobalParameter();

    ParticleInit(); /// パーティクル初期化
}

void EnemyManager::FSpawn() {
}

///========================================================================================
///  敵の生成
///========================================================================================
void EnemyManager::SpawnEnemy(const std::string& enemyType, const Vector3& position, const int32_t& groupID) {

    std::unique_ptr<BaseEnemy> enemy;

    if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]) { // 通常敵
        enemy = std::make_unique<NormalEnemy>();
        enemy->SetParameter(BaseEnemy::Type::NORMAL, paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
    }
    if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::STRONG)]) { // 通常敵
        enemy = std::make_unique<StrongEnemy>();
        enemy->SetParameter(BaseEnemy::Type::STRONG, paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
    }

    // 位置初期化とlistに追加
    enemy->SetPlayer(pPlayer_); // プレイヤーセット
    enemy->SetGameCamera(pGameCamera_);
    enemy->SetManager(this);
    enemy->SetCombo(pCombo_);
    enemy->SetGroupId(groupID);
    enemy->Init(position);
    enemies_.push_back(std::move(enemy));
}

///========================================================================================
///  更新処理
///========================================================================================
void EnemyManager::Update() {

    // SpawnUpdate(); // スポーン更新

    for (auto it = enemies_.begin(); it != enemies_.end();) {

        if ((*it)->GetType() == BaseEnemy::Type::NORMAL) {
            (*it)->SetParameter(BaseEnemy::Type::NORMAL, paramaters_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]);
        } else if ((*it)->GetType() == BaseEnemy::Type::STRONG) {
            (*it)->SetParameter(BaseEnemy::Type::STRONG, paramaters_[static_cast<size_t>(BaseEnemy::Type::STRONG)]);
        }

        (*it)->Update(); // 更新

        if ((*it)->GetIsDeath()) {

            if (pEnemySpawner_) {
                pEnemySpawner_->OnEnemyDestroyed((*it)->GetGroupId());
            }

            pLockOn_->OnEnemyDestroyed((*it).get());
            it = enemies_.erase(it); // 削除して次の要素を指すイテレータを取得
            GetIsEnemiesCleared(); // フラグを更新
        } else {
            ++it; // 削除しない場合はイテレータを進める
        }
    }
    ParticleUpdate(); // パーティクル更新
}

void EnemyManager::HpBarUpdate(const ViewProjection& viewProjection) {
    // if (isEditorMode_) {
    //	return; // エディタモード中は停止
    // }

    for (auto it = enemies_.begin(); it != enemies_.end();) {
        (*it)->DisplaySprite(viewProjection); // 更新
        ++it;
    }
}

///========================================================================================
///  描画処理
///========================================================================================
void EnemyManager::Draw(const ViewProjection& viewProjection) {
    for (auto it = enemies_.begin(); it != enemies_.end(); ++it) {
        (*it)->Draw(viewProjection);
    }
}

///========================================================================================
///  スプライト描画処理
///========================================================================================
void EnemyManager::SpriteDraw(const ViewProjection& viewProjection) {
    for (auto it = enemies_.begin(); it != enemies_.end(); ++it) {
        (*it)->SpriteDraw(viewProjection);
    }
}

///=================================================================================
/// ロード
///=================================================================================
void EnemyManager::ParamLoadForImGui() {

    /// ロードボタン
    if (ImGui::Button(std::format("Load {}", groupName_).c_str())) {

        globalParameter_->LoadFile(groupName_);
        /// セーブ完了メッセージ
        ImGui::Text("Load Successful: %s", groupName_.c_str());
        ApplyGlobalParameter();
    }
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void EnemyManager::AddParamGroup() {

    for (uint32_t i = 0; i < paramaters_.size(); ++i) {
        globalParameter_->AddItem(groupName_, "chaseDistance" + std::to_string(int(i + 1)), paramaters_[i].chaseDistance);
        globalParameter_->AddItem(groupName_, "chaseSpeed" + std::to_string(int(i + 1)), paramaters_[i].chaseSpeed);
        globalParameter_->AddItem(groupName_, "basePosY_" + std::to_string(int(i + 1)), paramaters_[i].basePosY);
        globalParameter_->AddItem(groupName_, "thrustRotateSpeed" + std::to_string(int(i + 1)), paramaters_[i].thrustRotateSpeed);
        globalParameter_->AddItem(groupName_, "upperGravity" + std::to_string(int(i + 1)), paramaters_[i].upperGravity);
        globalParameter_->AddItem(groupName_, "upperJumpPower" + std::to_string(int(i + 1)), paramaters_[i].upperJumpPower);
        globalParameter_->AddItem(groupName_, "upperFallSpeedLimit" + std::to_string(int(i + 1)), paramaters_[i].upperFallSpeedLimit);
        globalParameter_->AddItem(groupName_, "hitbackRotateTime" + std::to_string(int(i + 1)), paramaters_[i].archingbackTime);
        globalParameter_->AddItem(groupName_, "hitbackRotateValue" + std::to_string(int(i + 1)), paramaters_[i].archingbackValue);
        globalParameter_->AddItem(groupName_, "hitbackRotateBackRatio" + std::to_string(int(i + 1)), paramaters_[i].archingbackRatio);
        globalParameter_->AddItem(groupName_, "blowValue_" + std::to_string(int(i + 1)), paramaters_[i].blowValue);
        globalParameter_->AddItem(groupName_, "blowValueY_" + std::to_string(int(i + 1)), paramaters_[i].blowValueY);
        globalParameter_->AddItem(groupName_, "blowTime" + std::to_string(int(i + 1)), paramaters_[i].blowTime);
        globalParameter_->AddItem(groupName_, "blowRotateSpeed" + std::to_string(int(i + 1)), paramaters_[i].blowRotateSpeed);
        globalParameter_->AddItem(groupName_, "burstTime" + std::to_string(int(i + 1)), paramaters_[i].burstTime);
        globalParameter_->AddItem(groupName_, "blowGravity" + std::to_string(int(i + 1)), paramaters_[i].blowGravity);
        globalParameter_->AddItem(groupName_, "initScale" + std::to_string(int(i + 1)), paramaters_[i].initScale_);
    }
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void EnemyManager::SetValues() {

    for (uint32_t i = 0; i < paramaters_.size(); ++i) {
        globalParameter_->SetValue(groupName_, "chaseDistance" + std::to_string(int(i + 1)), paramaters_[i].chaseDistance);
        globalParameter_->SetValue(groupName_, "chaseSpeed" + std::to_string(int(i + 1)), paramaters_[i].chaseSpeed);
        globalParameter_->SetValue(groupName_, "basePosY_" + std::to_string(int(i + 1)), paramaters_[i].basePosY);
        globalParameter_->SetValue(groupName_, "thrustRotateSpeed" + std::to_string(int(i + 1)), paramaters_[i].thrustRotateSpeed);
        globalParameter_->SetValue(groupName_, "upperGravity" + std::to_string(int(i + 1)), paramaters_[i].upperGravity);
        globalParameter_->SetValue(groupName_, "upperJumpPower" + std::to_string(int(i + 1)), paramaters_[i].upperJumpPower);
        globalParameter_->SetValue(groupName_, "upperFallSpeedLimit" + std::to_string(int(i + 1)), paramaters_[i].upperFallSpeedLimit);
        globalParameter_->SetValue(groupName_, "hitbackRotateTime" + std::to_string(int(i + 1)), paramaters_[i].archingbackTime);
        globalParameter_->SetValue(groupName_, "hitbackRotateValue" + std::to_string(int(i + 1)), paramaters_[i].archingbackValue);
        globalParameter_->SetValue(groupName_, "hitbackRotateBackRatio" + std::to_string(int(i + 1)), paramaters_[i].archingbackRatio);
        globalParameter_->SetValue(groupName_, "blowValue_" + std::to_string(int(i + 1)), paramaters_[i].blowValue);
        globalParameter_->SetValue(groupName_, "blowValueY_" + std::to_string(int(i + 1)), paramaters_[i].blowValueY);
        globalParameter_->SetValue(groupName_, "blowTime" + std::to_string(int(i + 1)), paramaters_[i].blowTime);
        globalParameter_->SetValue(groupName_, "blowRotateSpeed" + std::to_string(int(i + 1)), paramaters_[i].blowRotateSpeed);
        globalParameter_->SetValue(groupName_, "burstTime" + std::to_string(int(i + 1)), paramaters_[i].burstTime);
        globalParameter_->SetValue(groupName_, "blowGravity" + std::to_string(int(i + 1)), paramaters_[i].blowGravity);
        globalParameter_->SetValue(groupName_, "initScale" + std::to_string(int(i + 1)), paramaters_[i].initScale_);
    }
}

///=====================================================
///  ImGuiからパラメータを得る
///=====================================================
void EnemyManager::ApplyGlobalParameter() {
    /// パラメータ
    for (uint32_t i = 0; i < paramaters_.size(); ++i) {
        paramaters_[i].chaseDistance       = globalParameter_->GetValue<float>(groupName_, "chaseDistance" + std::to_string(int(i + 1)));
        paramaters_[i].chaseSpeed          = globalParameter_->GetValue<float>(groupName_, "chaseSpeed" + std::to_string(int(i + 1)));
        paramaters_[i].basePosY            = globalParameter_->GetValue<float>(groupName_, "basePosY_" + std::to_string(int(i + 1)));
        paramaters_[i].thrustRotateSpeed   = globalParameter_->GetValue<float>(groupName_, "thrustRotateSpeed" + std::to_string(int(i + 1)));
        paramaters_[i].upperGravity        = globalParameter_->GetValue<float>(groupName_, "upperGravity" + std::to_string(int(i + 1)));
        paramaters_[i].upperJumpPower      = globalParameter_->GetValue<float>(groupName_, "upperJumpPower" + std::to_string(int(i + 1)));
        paramaters_[i].upperFallSpeedLimit = globalParameter_->GetValue<float>(groupName_, "upperFallSpeedLimit" + std::to_string(int(i + 1)));
        paramaters_[i].archingbackValue    = globalParameter_->GetValue<float>(groupName_, "hitbackRotateValue" + std::to_string(int(i + 1)));
        paramaters_[i].archingbackRatio    = globalParameter_->GetValue<float>(groupName_, "hitbackRotateBackRatio" + std::to_string(int(i + 1)));
        paramaters_[i].archingbackTime     = globalParameter_->GetValue<float>(groupName_, "hitbackRotateTime" + std::to_string(int(i + 1)));
        paramaters_[i].blowValue           = globalParameter_->GetValue<float>(groupName_, "blowValue_" + std::to_string(int(i + 1)));
        paramaters_[i].blowValueY          = globalParameter_->GetValue<float>(groupName_, "blowValueY_" + std::to_string(int(i + 1)));
        paramaters_[i].blowTime            = globalParameter_->GetValue<float>(groupName_, "blowTime" + std::to_string(int(i + 1)));
        paramaters_[i].blowRotateSpeed     = globalParameter_->GetValue<float>(groupName_, "blowRotateSpeed" + std::to_string(int(i + 1)));
        paramaters_[i].blowGravity         = globalParameter_->GetValue<float>(groupName_, "blowGravity" + std::to_string(int(i + 1)));
        paramaters_[i].burstTime           = globalParameter_->GetValue<float>(groupName_, "burstTime" + std::to_string(int(i + 1)));
        paramaters_[i].initScale_          = globalParameter_->GetValue<Vector3>(groupName_, "initScale" + std::to_string(int(i + 1)));
    }
}

void EnemyManager::DrawEnemyParamUI(BaseEnemy::Type type) {

    ImGui::DragFloat3("initScale", &paramaters_[static_cast<size_t>(type)].initScale_.x, 0.01f);
    ImGui::DragFloat("ChaseSpeed", &paramaters_[static_cast<size_t>(type)].chaseSpeed, 0.01f);
    ImGui::DragFloat("ChaseDistance", &paramaters_[static_cast<size_t>(type)].chaseDistance, 0.01f);
    ImGui::DragFloat("basePosY", &paramaters_[static_cast<size_t>(type)].basePosY, 0.01f);
    ImGui::DragFloat("thrustRotateSpeed", &paramaters_[static_cast<size_t>(type)].thrustRotateSpeed, 0.01f);
    ImGui::DragFloat("upperGravity", &paramaters_[static_cast<size_t>(type)].upperGravity, 0.01f);
    ImGui::DragFloat("upperJumpPowe", &paramaters_[static_cast<size_t>(type)].upperJumpPower, 0.01f);
    ImGui::DragFloat("upperFallSpeedLimit", &paramaters_[static_cast<size_t>(type)].upperFallSpeedLimit, 0.01f);
    ImGui::SliderAngle("hitbackRotateValue", &paramaters_[static_cast<size_t>(type)].archingbackValue, -360.0f, 720.0f);
    ImGui::DragFloat("hitbackRotateBackRatio", &paramaters_[static_cast<size_t>(type)].archingbackRatio, 0.01f);
    ImGui::DragFloat("hitbackRotateTime", &paramaters_[static_cast<size_t>(type)].archingbackTime, 0.01f);
    ImGui::DragFloat("blowValue", &paramaters_[static_cast<size_t>(type)].blowValue, 0.01f);
    ImGui::DragFloat("blowValueY", &paramaters_[static_cast<size_t>(type)].blowValueY, 0.01f);
    ImGui::DragFloat("blowRotateSpeed", &paramaters_[static_cast<size_t>(type)].blowRotateSpeed, 0.01f);
    ImGui::DragFloat("burstTime", &paramaters_[static_cast<size_t>(type)].burstTime, 0.01f);
    ImGui::DragFloat("blowGravity", &paramaters_[static_cast<size_t>(type)].blowGravity, 0.01f);
    ImGui::DragFloat("blowTime", &paramaters_[static_cast<size_t>(type)].blowTime, 0.01f);
}

void EnemyManager::AdjustParam() {
    SetValues();
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
        ParamLoadForImGui();

        ImGui::PopID();
    }

#endif // _DEBUG
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

    // ヒビ
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

void EnemyManager::SetPlayer(Player* player) {
    pPlayer_ = player;
}
void EnemyManager::SetLockon(LockOn* lockOn) {
    pLockOn_ = lockOn;
}
void EnemyManager::SetCombo(Combo* lockOn) {
    pCombo_ = lockOn;
}

void EnemyManager::GetIsEnemiesCleared() {
    areAllEnemiesCleared_ = enemies_.empty(); // 現在の敵リストが空かを確認
}

void EnemyManager::SetGameCamera(GameCamera* gamecamera) {
    pGameCamera_ = gamecamera;
}

void EnemyManager::SetEnemySpawner(EnemySpawner* enemySpawner) {
    pEnemySpawner_ = enemySpawner;
}
