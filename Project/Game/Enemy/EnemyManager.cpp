#include "EnemyManager.h"
// Types
#include "Types/NormalEnemy.h"
#include "Types/StrongEnemy.h"
// Player
#include "Player/Player.h"
// DeathTimer
#include "DeathTimer/DeathTimer.h"
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

    // HPバー色設定初期化
    hpBarColorConfig_.Init();

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

    // HPバー色設定を渡す
    enemy->SetHPBarColorConfig(&hpBarColorConfig_);

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

            if (pDeathTimer_) {
                int32_t comboCount = pCombo_ ? pCombo_->GetComboCount() : 0;
                pDeathTimer_->OnEnemyKilled(enemies_[i]->GetParameter().gaugeIncreaseValue, comboCount);
            }

            // vectorから削除
            enemies_[i] = std::move(enemies_.back());
            enemies_.pop_back();

            CheckIsEnemiesCleared();
        } else {
            ++i;
        }
    }
}

void EnemyManager::HpBarUpdate(const KetaEngine::ViewProjection& viewProjection) {
    Vector3 playerPos = pPlayer_ ? pPlayer_->GetBaseTransform().GetWorldPos() : Vector3{};

    for (size_t i = 0; i < enemies_.size(); ++i) {
        Vector3 enemyPos = enemies_[i]->GetWorldPosition();
        float dist = (enemyPos - playerPos).Length();

        if (dist <= hpBarDisplayDistance_) {
            enemies_[i]->DisplaySprite(viewProjection);
        } else {
            enemies_[i]->HideHpBar();
        }
    }
}

///=================================================================================
/// パラメータをグループに追加
///=================================================================================
void EnemyManager::RegisterParams() {

    // HPバー表示距離（全敵共通）
    globalParameter_->Regist(groupName_, "hpBarDisplayDistance", &hpBarDisplayDistance_);

    for (uint32_t i = 0; i < parameters_.size(); ++i) {
        const std::string& indexString = std::to_string(static_cast<int>(i + 1));

        // 基本パラメータ
        globalParameter_->Regist(groupName_, "basePosY_" + indexString, &parameters_[i].basePosY);
        globalParameter_->Regist(groupName_, "burstTime" + indexString, &parameters_[i].burstTime);
        globalParameter_->Regist(groupName_, "initScale" + indexString, &parameters_[i].baseScale_);
        globalParameter_->Regist(groupName_, "collisionRad" + indexString, &parameters_[i].collisionRad);
        globalParameter_->Regist(groupName_, "collisionOffset" + indexString, &parameters_[i].collisionOffset);
        globalParameter_->Regist(groupName_, "hpBarPosOffset" + indexString, &parameters_[i].hpBarPosOffset);

        // 追跡パラメータ
        globalParameter_->Regist(groupName_, "chaseDistanceMin" + indexString, &parameters_[i].chaseDistanceMin);
        globalParameter_->Regist(groupName_, "chaseSpeed" + indexString, &parameters_[i].chaseSpeed);
        globalParameter_->Regist(groupName_, "chaseDistance" + indexString, &parameters_[i].chaseDistance);

        // 死亡パラメータ
        globalParameter_->Regist(groupName_, "deathBlowValue" + indexString, &parameters_[i].deathBlowValue);
        globalParameter_->Regist(groupName_, "deathBlowValueY" + indexString, &parameters_[i].deathBlowValueY);
        globalParameter_->Regist(groupName_, "deathGravity" + indexString, &parameters_[i].deathGravity);
        globalParameter_->Regist(groupName_, "deathRotateSpeed" + indexString, &parameters_[i].deathRotateSpeed);
        globalParameter_->Regist(groupName_, "deathBurstTime" + indexString, &parameters_[i].deathBurstTime);
        // ゲージ増加値
        globalParameter_->Regist(groupName_, "gaugeIncreaseValue" + indexString, &parameters_[i].gaugeIncreaseValue);
    }
}

void EnemyManager::DrawEnemyParamUI(BaseEnemy::Type type) {
    const size_t typeIndex = static_cast<size_t>(type);

    ImGui::SeparatorText("基本パラメータ");
    ImGui::DragFloat3("initScale", &parameters_[typeIndex].baseScale_.x, 0.01f);
    ImGui::DragFloat("collisionRad", &parameters_[typeIndex].collisionRad, 0.1f, 0.1f, 20.0f);
    ImGui::DragFloat3("collisionOffset", &parameters_[typeIndex].collisionOffset.x, 0.1f);
    ImGui::DragFloat("basePosY", &parameters_[typeIndex].basePosY, 0.01f);
    ImGui::DragFloat("burstTime", &parameters_[typeIndex].burstTime, 0.01f);

    ImGui::SeparatorText("追跡パラメータ");
    ImGui::DragFloat("追跡開始距離", &parameters_[typeIndex].chaseDistance, 0.01f, 0.0f);
    ImGui::DragFloat("追跡限度", &parameters_[typeIndex].chaseDistanceMin, 0.1f, 0.0f);
    ImGui::DragFloat("追跡速度", &parameters_[typeIndex].chaseSpeed, 0.01f, 0.0f);

    ImGui::SeparatorText("死亡パラメータ");
    ImGui::DragFloat("DeathBlowValue", &parameters_[typeIndex].deathBlowValue, 0.1f);
    ImGui::DragFloat("DeathBlowValueY", &parameters_[typeIndex].deathBlowValueY, 0.1f);
    ImGui::DragFloat("DeathGravity", &parameters_[typeIndex].deathGravity, 0.1f);
    ImGui::DragFloat("DeathRotateSpeed", &parameters_[typeIndex].deathRotateSpeed, 0.01f);
    ImGui::DragFloat("DeathBurstTime", &parameters_[typeIndex].deathBurstTime, 0.01f);

    ImGui::SeparatorText("ゲージ設定");
    ImGui::DragFloat("ゲージ増加値", &parameters_[typeIndex].gaugeIncreaseValue, 0.01f, 0.0f);

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

        ImGui::SeparatorText("HPバー設定");
        ImGui::DragFloat("HpBar表示距離", &hpBarDisplayDistance_, 0.5f, 0.0f, 200.0f);
        hpBarColorConfig_.AdjustParam();

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

void EnemyManager::SetDeathTimer(DeathTimer* deathTimer) {
    pDeathTimer_ = deathTimer;
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